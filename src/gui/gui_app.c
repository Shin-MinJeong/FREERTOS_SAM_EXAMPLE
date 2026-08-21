#include "main.h"
#define IMG_COUNT   10

/* 변환된 이미지 선언 */
LV_IMG_DECLARE(cat1);  LV_IMG_DECLARE(cat2);
LV_IMG_DECLARE(cat3);  LV_IMG_DECLARE(cat4);
LV_IMG_DECLARE(cat5);  LV_IMG_DECLARE(cat6);
LV_IMG_DECLARE(cat7);  LV_IMG_DECLARE(cat8);
LV_IMG_DECLARE(cat9);  LV_IMG_DECLARE(cat10);

typedef struct {
    const char *         name;
    const lv_img_dsc_t * dsc;
} img_item_t;

static const img_item_t img_table[IMG_COUNT] = {
    { "Cat1",  &cat1  },  { "Cat2",  &cat2  },
    { "Cat3",  &cat3  },  { "Cat4",  &cat4  },
    { "Cat5",  &cat5  },  { "Cat6",  &cat6  },
    { "Cat7",  &cat7  },  { "Cat8",  &cat8  },
    { "Cat9",  &cat9  },  { "Cat10", &cat10 },
};

static lv_obj_t * g_scr_list = NULL;   /* 리스트 화면 */
static lv_obj_t * g_scr_view = NULL;   /* 이미지 화면 */
static lv_obj_t * g_img      = NULL;
static lv_obj_t * g_title    = NULL;

/* 뒤로가기 */
static void back_event_cb(lv_event_t * e)
{
    lv_scr_load(g_scr_list);
}

/* 리스트 항목 클릭 */
static void list_btn_event_cb(lv_event_t * e)
{
    uint32_t idx = (uint32_t)(uintptr_t)lv_event_get_user_data(e);
    if (idx >= IMG_COUNT) return;

    lv_img_set_src(g_img, img_table[idx].dsc);
    lv_label_set_text(g_title, img_table[idx].name);
    lv_scr_load(g_scr_view);

    printf("[LIST] idx=%lu, %s\r\n", (unsigned long)idx, img_table[idx].name);
}

static void create_list_screen(void)
{
    g_scr_list = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(g_scr_list, lv_color_hex(0xE0E0E0), 0);

    lv_obj_t * list = lv_list_create(g_scr_list);
    lv_obj_set_size(list, ILI9488_LCD_WIDTH - 20, ILI9488_LCD_HEIGHT - 20);
    lv_obj_center(list);

    lv_list_add_text(list, "Images");

    for (uint32_t i = 0; i < IMG_COUNT; i++) {
        lv_obj_t * btn = lv_list_add_btn(list, LV_SYMBOL_IMAGE, img_table[i].name);
        lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED,
                            (void *)(uintptr_t)i);
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
    create_list_screen();
    create_view_screen();
    lv_scr_load(g_scr_list);
}