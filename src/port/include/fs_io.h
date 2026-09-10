/*
 * fs_io.h
 *
 * FatFs 청크 전송 헬퍼.
 *
 * 리팩토링 이전에는 "32바이트 정렬 512B 버퍼 + 청크 루프"가 이미지
 * 로더와 이미지 덤프 두 곳에 각각 복사되어 있었다(버퍼도 2개, 총 1KB).
 * 구현을 하나로 합쳐 RAM 512바이트와 중복 로직을 함께 제거한다.
 *
 * 정렬 버퍼를 경유하는 이유: HSMCI/XDMAC 전송은 정렬된 버퍼를 요구하는데
 * 호출자가 넘기는 목적지(예: lv_mem_alloc 결과)는 정렬을 보장하지 않는다.
 *
 * [주의] conf_fatfs.h 의 _FS_REENTRANT 가 0 이므로 FatFs 자체가
 *        재진입 불가다. 따라서 이 모듈도 동시에 한 태스크에서만
 *        호출해야 한다. 현 설계는 "마운트 태스크 종료 -> GUI 태스크가
 *        파일 접근" 순서로 직렬화되어 이 전제를 만족한다.
 */

#ifndef PORT_FS_IO_H_
#define PORT_FS_IO_H_

#include <stdbool.h>
#include <stdint.h>

#include "ff.h"

/** 정렬 바운스 버퍼 크기 (SD 섹터 1개). */
#define FS_IO_CHUNK_SIZE 512u

/**
 * \brief 파일에서 정확히 size 바이트를 읽어 dst 로 옮긴다.
 *
 * \return size 바이트를 모두 읽었으면 true, 그 외 false
 */
bool fs_io_read_exact(FIL *file, void *dst, uint32_t size);

/**
 * \brief src 의 size 바이트를 파일에 정확히 기록한다.
 *
 * \return size 바이트를 모두 기록했으면 true, 그 외 false
 */
bool fs_io_write_exact(FIL *file, const void *src, uint32_t size);

#endif /* PORT_FS_IO_H_ */
