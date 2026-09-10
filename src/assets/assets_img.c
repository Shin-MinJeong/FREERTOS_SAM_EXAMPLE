/*
 * assets_img.c
 *
 * === 왜 .c 파일을 #include 하는가 ===
 * img_cat*.c 는 LVGL 이미지 변환기가 생성한 파일이라 손으로 조건부
 * 컴파일 가드를 넣을 수 없다(재생성하면 지워진다). 한편 Atmel Studio
 * 프로젝트(.cproj)는 파일 단위 컴파일 여부를 매크로로 제어할 수 없다.
 *
 * 그래서 생성 파일들을 독립 번역 단위에서 빼고 이 파일 안으로 포함해,
 * APP_ENABLE_IMAGE_EXPORT 하나로 "약 800KB 를 플래시에 넣을지" 를
 * 결정할 수 있게 한다. img_cat*.c 는 .cproj 에서 컴파일 대상이 아니라
 * 리소스(None)로 등록되어 있다.
 */

#include "main.h"

#if APP_ENABLE_IMAGE_EXPORT

/* 생성된 이미지 데이터. 각 파일은 자기 심볼만 정의하므로 한 번역
 * 단위에 모아도 충돌하지 않는다. */
#include "img_cat1.c"
#include "img_cat2.c"
#include "img_cat3.c"
#include "img_cat4.c"
#include "img_cat5.c"
#include "img_cat6.c"
#include "img_cat7.c"
#include "img_cat8.c"
#include "img_cat9.c"
#include "img_cat10.c"

/* SD 카드에 만들어질 파일 이름과 원본의 대응. */
static const assets_img_item_t IMAGE_TABLE[] = {
	{ "CAT1",  &cat1  },
	{ "CAT2",  &cat2  },
	{ "CAT3",  &cat3  },
	{ "CAT4",  &cat4  },
	{ "CAT5",  &cat5  },
	{ "CAT6",  &cat6  },
	{ "CAT7",  &cat7  },
	{ "CAT8",  &cat8  },
	{ "CAT9",  &cat9  },
	{ "CAT10", &cat10 },
};

const assets_img_item_t *assets_img_table(uint32_t *count)
{
	*count = (uint32_t)(sizeof(IMAGE_TABLE) / sizeof(IMAGE_TABLE[0]));

	return IMAGE_TABLE;
}

#endif /* APP_ENABLE_IMAGE_EXPORT */
