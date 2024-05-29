#pragma once
#include <kutils/bitmap.hpp>
#include <libc/stdint.hpp>
#include <memory/mmap.hpp>

enum PMemConsts {
	FRAME_SZ = 4096,
};

class PMem {
  private:
	size __last_free_idx = 0;
	size __free_frames;				   // Number of free frames
	size __locked_frames;			   // Number of locked frames
	size __frames;					   // Total amount of frames
	Bitmap __bitmap = __init_bitmap(); // Frames bitmap

  public:
	const size &frames		  = __frames;
	const size &locked_frames = __locked_frames;
	const size &free_frames	  = __free_frames;

  private:
	Bitmap __init_bitmap();

  public:
	PMem();
	
	void lock_frame(size idx);
	void free_frame(size idx);
	u8 is_frame_locked(size idx);
	
	void *request_frame();
	void *request_frames(size number);
	
	static void *idx_to_frame_ptr(size idx);
	static size ptr_to_frame_idx(const void *mem_ptr);
};