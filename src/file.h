/// IMS 2024
/// @author: Daria Kinash(xkinas00)

#ifndef FILE_H
#define FILE_H

#include "data.h"
#include <sys/stat.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void aggregate_consumption(const char *filename, YearData data[], int *years, int max_years);
void load_investment(const char *filename, YearData data[], int years);
void load_gdp(const char *filename, YearData data[], int years);
void create_directory(const char *dirname);

#endif