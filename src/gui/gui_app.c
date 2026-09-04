#include "main.h"

#define MAX_IMAGES 30

// ===== 플래시에 있는 원본 이미지 소스 (백업용) =====
typedef struct {
	const char *name;              // 저장할 때 쓸 이름 (확장자 없이)
	const lv_img_dsc_t *dsc;       // 플래시에 있는 LVGL 이미지 디스크립터
} flash_img_item_t;

extern const lv_img_dsc_t cat1;
extern const lv_img_dsc_t cat2;
extern const lv_img_dsc_t cat3;
extern const lv_img_dsc_t cat4;
extern const lv_img_dsc_t cat5;
extern const lv_img_dsc_t cat6;
extern const lv_img_dsc_t cat7;
extern const lv_img_dsc_t cat8;
extern const lv_img_dsc_t cat9;
extern const lv_img_dsc_t cat10;

static const flash_img_item_t flash_img_table[] = {
	{ "CAT1", &cat1 },
	{ "CAT2", &cat2 },
	{ "CAT3", &cat3 },
	{ "CAT4", &cat4 },
	{ "CAT5", &cat5 },
	{ "CAT6", &cat6 },
	{ "CAT7", &cat7 },
	{ "CAT8", &cat8 },
	{ "CAT9", &cat9 },
	{ "CAT10", &cat10 }
};

#define IMG_COUNT (sizeof(flash_img_table) / sizeof(flash_img_table[0]))
//////////////////////////////////////////////////////
typedef struct {
	char name[32]; 
	char path[32]; 
} img_item_t;

static img_item_t img_table[MAX_IMAGES];
static uint32_t img_count = 0;

static lv_obj_t * g_scr_list = NULL;
static lv_obj_t * g_scr_view = NULL;
static lv_obj_t * g_img      = NULL;
static lv_obj_t * g_title    = NULL;

void scan_sd_card_for_images(void)
{
	DIR dir;
	FILINFO fno;
	FRESULT res;

	img_count = 0;
	
	// FatFs의 0번 드라이브(SD카드 루트) 열기
	res = f_opendir(&dir, "0:");
	if (res == FR_OK) {
		while (1) {
			// 파일 하나 읽기
			res = f_readdir(&dir, &fno);
			
			// 더 이상 파일이 없거나 에러면 스캔 종료
			if (res != FR_OK || fno.fname[0] == 0) break;
			
			// 디렉토리(폴더)면 건너뛰기
			if (fno.fattrib & AM_DIR) continue;

			// 파일명에 ".bin"이나 ".BIN"이 포함되어 있는지 확인
			char *ext = strrchr(fno.fname, '.');
			if (ext && (strcmp(ext, ".bin") == 0 || strcmp(ext, ".BIN") == 0)) {
				if (img_count < MAX_IMAGES) {
					// 1. LVGL이 읽을 전체 경로 만들기 ("S:파일명.bin")
					sprintf(img_table[img_count].path, "S:%s", fno.fname);
					
					char *path_dot = strrchr(img_table[img_count].path, '.');
					if (path_dot) {
						for (char *p = path_dot; *p; p++) 
							*p = tolower((unsigned char)*p);
					}

					//for (char *p = img_table[img_count].path; *p; p++) {
						//*p = tolower((unsigned char)*p);
					//}

					// 2. UI에 표시할 이름 만들기 (확장자 .bin 떼어내기)
					strncpy(img_table[img_count].name, fno.fname, 31);
					char *dot = strrchr(img_table[img_count].name, '.');
					if (dot) *dot = '\0'; // 온점(.) 위치에서 문자열 자르기
					
					printf("[SCAN] Found Image: %s -> %s\r\n", fno.fname, img_table[img_count].path);
					img_count++;
				}
			}
		}
		} else {
		printf("[SCAN] Failed to open directory! Error: %d\r\n", res);
	}
}


/* 뒤로가기 */
static void back_event_cb(lv_event_t * e)
{
	lv_img_cache_invalidate_src(lv_img_get_src(g_img)); // 메모리 최적화
	lv_scr_load(g_scr_list);
}

/* 리스트 항목 클릭 */
static void list_btn_event_cb(lv_event_t * e)
{
	uint32_t idx = (uint32_t)(uintptr_t)lv_event_get_user_data(e);
	if (idx >= img_count) return;

	printf("[LIST] Loading %s from %s ...\r\n", img_table[idx].name, img_table[idx].path);

	lv_img_set_src(g_img, img_table[idx].path);
	lv_label_set_text(g_title, img_table[idx].name);
	lv_scr_load(g_scr_view);
	
	lv_mem_monitor_t mon;
	lv_mem_monitor(&mon);
	printf("[MEM] total=%u, free=%u, used_pct=%u%%, max_used=%u\r\n", (unsigned)mon.total_size, (unsigned)mon.free_size, mon.used_pct, (unsigned)mon.max_used);
	   
}

static void create_list_screen(void)
{
	g_scr_list = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(g_scr_list, lv_color_hex(0xE0E0E0), 0);

	lv_obj_t * list = lv_list_create(g_scr_list);
	lv_obj_set_size(list, ILI9488_LCD_WIDTH - 20, ILI9488_LCD_HEIGHT - 20);
	lv_obj_center(list);

	lv_list_add_text(list, "SD Card Images");

	if (img_count == 0) {
		lv_list_add_text(list, "No .bin files found!");
		} else {
		for (uint32_t i = 0; i < img_count; i++) {
			lv_obj_t * btn = lv_list_add_btn(list, LV_SYMBOL_IMAGE, img_table[i].name);
			lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)i);
		}
	}
}

static void create_view_screen(void)
{
	g_scr_view = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(g_scr_view, lv_color_hex(0x202020), 0);

	g_title = lv_label_create(g_scr_view);
	lv_obj_set_style_text_color(g_title, lv_color_hex(0xFFFFFF), 0);
	lv_obj_align(g_title, LV_ALIGN_TOP_MID, 0, 10);

	g_img = lv_img_create(g_scr_view);
	lv_obj_center(g_img);

	lv_obj_t * back = lv_btn_create(g_scr_view);
	lv_obj_set_size(back, 100, 45);
	lv_obj_align(back, LV_ALIGN_BOTTOM_MID, 0, -10);
	lv_obj_add_event_cb(back, back_event_cb, LV_EVENT_CLICKED, NULL);

	lv_obj_t * lbl = lv_label_create(back);
	lv_label_set_text(lbl, LV_SYMBOL_LEFT " Back");
	lv_obj_center(lbl);
}

void gui_app_create_ui(void)
{
	if (xSemaphoreTake(sdMountedSem, pdMS_TO_TICKS(5000)) != pdTRUE) {
		printf("[GUI] WARNING: SD mount wait timeout!\n");
	}
	
	scan_sd_card_for_images(); 
	
	create_list_screen();
	create_view_screen();
	lv_scr_load(g_scr_list);
}

// 이미지 백업용 임시 함수 ...
void dump_images_to_sd(void)
{
	FIL file;
	UINT bw;
	char filename[48];

	printf("--- Start Image Backup to SD Card ---\n");

	for (uint32_t i = 0; i < IMG_COUNT; i++) {
		const flash_img_item_t *item = &flash_img_table[i];

		printf("[DUMP] %s : w=%d h=%d data_size=%lu\n",
		item->name,
		item->dsc->header.w,
		item->dsc->header.h,
		(unsigned long)item->dsc->data_size);

		sprintf(filename, "0:%s.bin", item->name);

		if (f_open(&file, filename, FA_CREATE_ALWAYS | FA_WRITE) == FR_OK) {

			FRESULT wres1 = f_write(&file, &item->dsc->header, sizeof(lv_img_header_t), &bw);
			printf("[DUMP] header write res=%d, bw=%u\n", wres1, bw);

			FRESULT wres2 = f_write(&file, item->dsc->data, item->dsc->data_size, &bw);
			printf("[DUMP] data write res=%d, requested=%lu, written=%u\n",
			wres2, (unsigned long)item->dsc->data_size, bw);

f_sync(&file);

			f_close(&file);
			printf("Saved: %s (Size: %lu bytes)\n", filename,
			(unsigned long)item->dsc->data_size + sizeof(lv_img_header_t));
			} else {
			printf("Failed to save: %s\n", filename);
		}
	}

	printf("--- Image Backup Finished! ---\n");
}