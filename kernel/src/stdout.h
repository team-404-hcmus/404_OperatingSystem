#ifndef __STDOUT_H__
#define __STDOUT_H__

#pragma once
//#include "stream/stream.h"
#include "Console.h"

class Endl
{

};
class Stdout
{
private:
	Console* _console;
	uint8_t buffer[1024];
	uint8_t head = 0;
	static const uint64_t buffer_size = 1024;
	uint32_t color = 0xffffff00;
	Stdout& print_num_int(Stdout& out,int64_t num);
	Stdout& print_num_uint(Stdout& out,uint64_t num);
public:
	void write(const void* byte, uint8_t size);
	void init(Console* console,uint32_t color);
	void flush();
	friend Stdout& operator<<(Stdout& out,const char* x);
	friend Stdout& operator<<(Stdout& out,uint64_t x);
	friend Stdout& operator<<(Stdout& out,int64_t x);
	friend Stdout& operator<<(Stdout& out,uint32_t x);
	friend Stdout& operator<<(Stdout& out,int32_t x);
	friend Stdout& operator<<(Stdout& out,uint16_t x);
	friend Stdout& operator<<(Stdout& out,int16_t x);
	friend Stdout& operator<<(Stdout& out,Endl x);
	friend Stdout& operator<<(Stdout& out,float x);
	friend Stdout& operator<<(Stdout& out,const char* x);
	friend Stdout& operator<<(Stdout& out,const char* x);
};


extern Endl endl;
extern Stdout stdout;
#endif // __STDOUT_H__