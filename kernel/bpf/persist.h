// SPDX-License-Identifier: GPL-2.0-only
/*
 * Functions to provide persistence to eBPF maps
 *
 * Copyright (c) 2024 Tanuj Ravi Rao
 */

#ifndef __BPF_PERSIST_H_
#define __BPF_PERSIST_H_

#include <linux/types.h>
#include <linux/fs.h>

/* 8-byte ring buffer record header structure, same as struct bpf_ringbuf_hdr */
struct bpf_ringbuf_record {
	u32 len;
	u32 pg_off;
};

/* enum of persistence types */
enum persist_type { BPF_PERSIST_VFS, BPF_PERSIST_NVME, BPF_PERSIST_PMEM };

/* struct to keep track of maps */
struct bpf_persist_map_hdr {
	u32 id;
	char name[16U];
	unsigned long cons_pos;
	unsigned long prod_pos;
	enum persist_type p_type;
};

/* struct for bpf_persistd data */
struct bpf_persistd_data {
	struct file *file;
	char * write_buffer; // start address of buffer that needs to be written
	u64 length;
	loff_t offset;
	bool done;
	spinlock_t lock;
};

void *bpf_ringbuf_restore_from_record(struct bpf_ringbuf_record *hdr);

/* bpf_persistd thread */
int bpf_persistd(void *);

/* function to init kthread for bpf_persistd */
int initialize_kthread(void);

/* open a file for persistence */
int bpf_persist_map_open(u32 id, char *name, void *rb_ptr, u32 size);

/* write the map_hdr section of the persistent file */
int __bpf_persist_map_write_hdr(void *rb_ptr);

/* commit the reserved region */
int bpf_persist_map_write(struct bpf_ringbuf_record *hdr,
			  unsigned long rec_idx);

/* close the file */
void bpf_persist_map_close(char *name);
#endif
