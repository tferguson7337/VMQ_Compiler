#ifndef FILE_IO_H_
#define FILE_IO_H_

#include <stdio.h>

int getFileIndex(int count, char*** strs);

void setSourceFile(int count, char*** args, FILE** fp);

void setDestFile(char* src_file_name, FILE** fp);

#endif
