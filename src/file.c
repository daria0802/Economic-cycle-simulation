/// IMS 2024
/// @author: Daria Kinash(xkinas00)

#include "file.h"

/**
 * Creates a directory if it does not already exist.
 * @param dirname The name of the directory.
 */
void create_directory(const char *dirname) {
    struct stat st = {0};
    if (stat(dirname, &st) == -1) {
        if (mkdir(dirname, 0700) != 0) {
            perror("Error creating directory");
            exit(1);
        }
    }
}

/**
 * Parses a CSV line into tokens.
 * @param line The line to be parsed.
 * @param tokens An array to store the parsed tokens.
 */
void parse_csv_line(const char *line, char tokens[][1024]) {
    int token_count = 0;
    int in_quotes = 0;
    char current_token[1024] = {0};
    int current_pos = 0;

    for (int i = 0; line[i] != '\0'; i++) {
        char c = line[i];
        if (c == '"' && (i == 0 || line[i - 1] != '\\')) {
            in_quotes = !in_quotes; 
        } else if (c == ',' && !in_quotes) {
            current_token[current_pos] = '\0';
            strncpy(tokens[token_count], current_token, 1024);
            token_count++;
            current_pos = 0;
            memset(current_token, 0, sizeof(current_token));
        } else {
            current_token[current_pos++] = c;
        }
    }

    current_token[current_pos] = '\0';
    strncpy(tokens[token_count], current_token, 1024);
    token_count++;
}

/**
 * Aggregates annual consumption from quarterly data in a CSV file.
 * @param filename The CSV file containing consumption data.
 * @param data Array to store the aggregated data.
 * @param years Pointer to the number of years processed.
 * @param max_years Capacity of the data array.
 */
void aggregate_consumption(const char *filename, YearData data[], int *years, int max_years) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(1);
    }

    char line[1024];
    fgets(line, sizeof(line), file);
    double quarterly[4] = {0};
    char current_year[10] = "";

    while (fgets(line, sizeof(line), file)) {
        char tokens[20][1024];
        parse_csv_line(line, tokens);

        char year[10] = "";
        char quarter[3] = "";
        sscanf(tokens[7], "%4[^-]-%2s", year, quarter);

        double value = atof(tokens[8]);

        if (strcmp(current_year, year) != 0) {
            if (current_year[0] != '\0') {
                if (*years >= max_years) {
                    fprintf(stderr, "Too many years in %s (max %d)\n", filename, max_years);
                    exit(1);
                }
                data[*years].year = atoi(current_year);
                for (int i = 0; i < 4; i++) {
                    data[*years].consumption += quarterly[i];
                    quarterly[i] = 0;
                }
                (*years)++;
            }
            strcpy(current_year, year);
        }

        int quarter_index = quarter[1] - '1';
        if (quarter_index >= 0 && quarter_index < 4) {
            quarterly[quarter_index] = value;
        }
    }

    if (current_year[0] != '\0' && *years < max_years) {
        data[*years].year = atoi(current_year);
        for (int i = 0; i < 4; i++) {
            data[*years].consumption += quarterly[i];
        }
        (*years)++;
    }

    fclose(file);
}

/**
 * Loads investment data from a CSV file and stores it in the YearData array.
 * @param filename The CSV file containing investment data.
 * @param data Array to store the investment data.
 * @param years The number of years already processed.
 */
void load_investment(const char *filename, YearData data[], int years) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(1);
    }

    char line[1024];
    fgets(line, sizeof(line), file); 

    while (fgets(line, sizeof(line), file)) {
        char tokens[20][1024];
        parse_csv_line(line, tokens);

        int year = atoi(tokens[6]);
        double value = atof(tokens[7]);

        for (int i = 0; i < years; i++) {
            if (data[i].year == year) {
                data[i].investment = value;
                break;
            }
        }
    }

    fclose(file);
}


/**
 * Loads GDP data from a CSV file and stores it in the YearData array.
 * @param filename The CSV file containing GDP data.
 * @param data Array to store the GDP data.
 * @param years The number of years already processed.
 */
void load_gdp(const char *filename, YearData data[], int years) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(1);
    }

    char line[1024];
    fgets(line, sizeof(line), file); 

    while (fgets(line, sizeof(line), file)) {
        char tokens[20][1024];
        parse_csv_line(line, tokens);

        int year = atoi(tokens[6]); 
        double value = atof(tokens[7]);

        for (int i = 0; i < years; i++) {
            if (data[i].year == year) {
                data[i].gdp = value;
                break;
            }
        }
    }

    fclose(file);
}