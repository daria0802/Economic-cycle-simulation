/// IMS 2024
/// @author: Daria Kinash(xkinas00)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include "file.h"

#define MAX_YEARS 20

void calculate_future_years(YearData data[], double a, double c, int total_years);
double calculate_pearson_correlation(YearData data[], int years);


int main() {
    YearData data[MAX_YEARS] = {0};
    int years = 0;

    create_directory("results");

    // Aggregate consumption
    aggregate_consumption("data/estat_consumption.csv", data, &years, MAX_YEARS);
    if (years < 2) {
        fprintf(stderr, "At least two years of data are required\n");
        return 1;
    }

    // Load investment data
    load_investment("data/estat_investment.csv", data, years);

    // Load GDP data
    load_gdp("data/estat_gdp.csv", data, years);

    data[0].income = data[0].consumption + data[0].investment;
    data[1].income = data[1].consumption + data[1].investment;

    // Parameter pairs {a, c}: a = accelerator (investment response to income change),
    // c = marginal propensity to consume (drives the multiplier effect)
    double parameters[][2] = {{0.5, 0.5}, {0, 1}, {1, 0}, {0.9, 0.1}, {1, 1}, {0.1, 0.1}, {0.9, 0.9}, {0.6, 0.3}, {0.3, 0.6}};
    int param_count = sizeof(parameters) / sizeof(parameters[0]);

    for (int i = 0; i < param_count; i++) {
        double a = parameters[i][0];
        double c = parameters[i][1];

        YearData temp_data[MAX_YEARS];
        memcpy(temp_data, data, sizeof(data));

        calculate_future_years(temp_data, a, c, years);

        char filename[100];
        sprintf(filename, "results/results_a%.1f_c%.1f.csv", a, c);
        FILE *file = fopen(filename, "w");
        if (!file) {
            perror("Error opening file");
            exit(1);
        }
        fprintf(file, "Year,Investment,Consumption,Income,GDP\n");
        for (int j = 0; j < years; j++) {
            fprintf(file, "%d,%.2f,%.2f,%.2f,%.2f\n",
                    temp_data[j].year,
                    temp_data[j].investment,
                    temp_data[j].consumption,
                    temp_data[j].income,
                    temp_data[j].gdp);
        }
        fclose(file);

        // Calculate Pearson correlation
        double correlation = calculate_pearson_correlation(temp_data, years);
        printf("Pearson correlation for a=%.1f, c=%.1f: %.4f\n", a, c, correlation);
    }

    return 0;
}

/**
 * Calculates future years' income, consumption, and investment based on the Samuelson-Hicks model.
 * @param data Array of data to store results.
 *   I(t) = a * (Y(t-1) - Y(t-2)),  C(t) = c * Y(t-1),  Y(t) = I(t) + C(t)
 * @param a Accelerator parameter.
 * @param c Marginal propensity to consume (multiplier parameter).
 * @param total_years Number of years to calculate.
 */
void calculate_future_years(YearData data[], double a, double c, int total_years) {
    for (int i = 2; i < total_years; i++) {
        double delta_income = data[i - 1].income - data[i - 2].income;
        data[i].investment = a * delta_income;
        data[i].consumption = c * data[i - 1].income;
        data[i].income = data[i].investment + data[i].consumption;
    }
}

/**
 * Calculates the Pearson correlation coefficient between income and GDP.
 * @param data Array of data containing income and GDP.
 * @param years Number of years in the dataset.
 * @return Pearson correlation coefficient.
 */
double calculate_pearson_correlation(YearData data[], int years) {
    double sum_x = 0, sum_y = 0, sum_xy = 0, sum_x2 = 0, sum_y2 = 0;
    int n = years;

    for (int i = 0; i < n; i++) {
        double x = data[i].income;
        double y = data[i].gdp;

        sum_x += x;
        sum_y += y;
        sum_xy += x * y;
        sum_x2 += x * x;
        sum_y2 += y * y;
    }

    double numerator = n * sum_xy - sum_x * sum_y;
    double denominator = sqrt((n * sum_x2 - sum_x * sum_x) * (n * sum_y2 - sum_y * sum_y));
    return denominator == 0 ? 0 : numerator / denominator; 
}
