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
	
	res = f_opendir(&dir, "0:");
	if (res == FR_OK) {
		while (1) {
			res = f_readdir(&dir, &fno);
			
			if (res != FR_OK || fno.fname[0] == 0) break;
			
			if (fno.fattrib & AM_DIR) continue;

			char *ext = strrchr(fno.fname, '.');
			if (ext && (strcmp(ext, ".bin") == 0 || strcmp(ext, ".BIN") == 0)) {
				if (img_count < MAX_IMAGES) {
					sprintf(img_table[img_count].path, "S:%s", fno.fname);
					
					char *path_dot = strrchr(img_table[img_count].path, '.');
					if (path_dot) {
						for (char *p = path_dot; *p; p++) 
							*p = tolower((unsigned char)*p);
					}

					strncpy(img_table[img_count].name, fno.fname, 31);
					char *dot = strrchr(img_table[img_count].name, '.');
					if (dot) *dot = '\0';
					
					printf("[SCAN] Found Image: %s -> %s\r\n", fno.fname, img_table[img_count].path);
					img_count++;
				}
			}
		}
		} else {
		printf("[SCAN] Failed to open directory! Error: %d\r\n", res);
	}
}


static lv_img_dsc_t  g_ram_dsc;
static uint8_t     * g_ram_buf = NULL;
static COMPILER_ALIGNED(32) uint8_t s_bounce[512];

/* path 예: "0:CAT5.bin"  (LVGL의 "S:" 가 아니라 FatFs 드라이브 번호) */
static bool load_bin_to_ram(const char *path)
{
    FIL f;  UINT br;  lv_img_header_t hdr;

    if (f_open(&f, path, FA_READ) != FR_OK) { printf("[RAM] open fail\r\n"); return false; }
    if (f_read(&f, &hdr, sizeof(hdr), &br) != FR_OK || br != sizeof(hdr)) { f_close(&f); return false; }

    uint32_t px   = lv_img_cf_get_px_size(hdr.cf) >> 3;      /* 바이트/픽셀 */
    uint32_t size = (uint32_t)hdr.w * hdr.h * px;

    /* 이전 이미지가 캐시에 남아있으면 무효화 후 해제 */
    if (g_ram_buf) {
        lv_img_cache_invalidate_src(&g_ram_dsc);
        lv_mem_free(g_ram_buf);
        g_ram_buf = NULL;
    }

    g_ram_buf = lv_mem_alloc(size);
    if (!g_ram_buf) { printf("[RAM] alloc FAIL %lu\r\n", (unsigned long)size); f_close(&f); return false; }

    uint32_t total = 0;
    while (total < size) {
        uint32_t chunk = (size - total > sizeof(s_bounce)) ? sizeof(s_bounce) : (size - total);
        if (f_read(&f, s_bounce, chunk, &br) != FR_OK || br == 0) break;
        memcpy(g_ram_buf + total, s_bounce, br);
        total += br;
    }
    f_close(&f);
	
    if (total != size) return false;

    g_ram_dsc.header    = hdr;
    g_ram_dsc.data_size = size;
    g_ram_dsc.data      = g_ram_buf;

    return true;
}

/* 뒤로가기 */
static void back_event_cb(lv_event_t * e)
{
	lv_img_cache_invalidate_src(lv_img_get_src(g_img)); // 메모리 최적화
	lv_scr_load(g_scr_list);
}

/* 리스트 항목 클릭 */
//static void list_btn_event_cb(lv_event_t * e)
//{
	//uint32_t idx = (uint32_t)(uintptr_t)lv_event_get_user_data(e);
	//if (idx >= img_count) return;
//
	//printf("[LIST] Loading %s from %s ...\r\n", img_table[idx].name, img_table[idx].path);
//
	//lv_img_set_src(g_img, img_table[idx].path);
	//lv_label_set_text(g_title, img_table[idx].name);
	//lv_scr_load(g_scr_view);
	//
	//lv_mem_monitor_t mon;
	//lv_mem_monitor(&mon);
	//printf("[MEM] total=%u, free=%u, used_pct=%u%%, max_used=%u\r\n", (unsigned)mon.total_size, (unsigned)mon.free_size, mon.used_pct, (unsigned)mon.max_used);
	   //
//}
static void list_btn_event_cb(lv_event_t * e)
{
    uint32_t idx = (uint32_t)(uintptr_t)lv_event_get_user_data(e);
    if (idx >= img_count) return;

    char fatfs_path[40];
    snprintf(fatfs_path, sizeof(fatfs_path), "0:%s", img_table[idx].path + 2);

    if (!load_bin_to_ram(fatfs_path)) {
        printf("[LIST] load failed\r\n");
        return;
    }

    lv_img_cache_invalidate_src(&g_ram_dsc);   /* 같은 포인터 재사용이므로 필수 */
    lv_img_set_src(g_img, &g_ram_dsc);
    lv_label_set_text(g_title, img_table[idx].name);
    lv_scr_load(g_scr_view);
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
static COMPILER_ALIGNED(32) uint8_t s_wbuf[512];

void dump_images_to_sd(void)
{
    FIL  f;
    UINT bw;
    char filename[48];

    printf("--- Start Image Backup to SD Card ---\n");

    for (uint32_t i = 0; i < IMG_COUNT; i++) {
        const flash_img_item_t *item = &flash_img_table[i];
        const uint8_t *src = (const uint8_t *)item->dsc->data;
        uint32_t remain    = item->dsc->data_size;
        uint32_t total     = 0;
        bool     ok        = true;

        snprintf(filename, sizeof(filename), "0:%s.bin", item->name);

        if (f_open(&f, filename, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) {
            printf("[DUMP] open fail: %s\n", filename);
            continue;
        }

        memcpy(s_wbuf, &item->dsc->header, sizeof(lv_img_header_t));
        if (f_write(&f, s_wbuf, sizeof(lv_img_header_t), &bw) != FR_OK
            || bw != sizeof(lv_img_header_t)) {
            printf("[DUMP] header write FAIL\n");
            ok = false;
        }

        while (ok && remain > 0) {
            uint32_t chunk = (remain > sizeof(s_wbuf)) ? sizeof(s_wbuf) : remain;

            memcpy(s_wbuf, src + total, chunk);     

            if (f_write(&f, s_wbuf, chunk, &bw) != FR_OK || bw != chunk) {
                printf("[DUMP] write FAIL at %lu (bw=%u)\n",
                       (unsigned long)total, bw);
                ok = false;
                break;
            }
            total  += chunk;
            remain -= chunk;
        }

        f_sync(&f);
        f_close(&f);

        printf("[DUMP] %s : %s (%lu / %lu bytes)\n", item->name, ok ? "OK" : "FAILED", (unsigned long)total, (unsigned long)item->dsc->data_size);
    }

    printf("--- Image Backup Finished! ---\n");
}

