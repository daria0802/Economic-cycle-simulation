# Czech Economic Cycle Simulation (Samuelson–Hicks Model)

A C program that simulates the national income of the Czech Republic from 2010 to 2023 using the Samuelson–Hicks multiplier–accelerator model. It runs on real Eurostat data, tests several parameter combinations, and checks each simulated income series against actual GDP with the Pearson correlation coefficient.

> Project for *Modelling and Simulation (IMS)*, Brno University of Technology, Faculty of Information Technology (2024).
> The full write-up, covering the model, hypotheses, experiments and interpretation, is in [`documentation.pdf`](documentation.pdf).

## The model

Starting from two years of real data, each following year is computed as:

```text
I(t) = a · (Y(t-1) − Y(t-2))     # investment: accelerator
C(t) = c · Y(t-1)                # consumption: marginal propensity to consume (multiplier)
Y(t) = I(t) + C(t)               # national income
```

The program runs 9 experiments with different `(a, c)` pairs and correlates each simulated `Y` with real GDP.

## Data

Eurostat datasets for Czechia (in `data/`):

| File | Dataset |
|---|---|
| `estat_consumption.csv` | Household final consumption, quarterly (aggregated to years) |
| `estat_investment.csv` | Gross fixed capital formation, annual |
| `estat_gdp.csv` | `NAMA_10_GDP`, GDP at market prices, annual |

## Build and run

```bash
make run
```

This writes one CSV file per experiment to `results/` (`results_a<a>_c<c>.csv`, with columns Year, Investment, Consumption, Income, GDP) and prints the correlations:

```text
Pearson correlation for a=0.5, c=0.5: -0.3128
Pearson correlation for a=0.0, c=1.0: 0.2588
Pearson correlation for a=1.0, c=0.0: 0.2234
Pearson correlation for a=0.9, c=0.1: 0.0843
Pearson correlation for a=1.0, c=1.0: 0.9159
Pearson correlation for a=0.1, c=0.1: -0.3478
Pearson correlation for a=0.9, c=0.9: -0.7986
Pearson correlation for a=0.6, c=0.3: -0.2044
Pearson correlation for a=0.3, c=0.6: -0.4765
```

Only the combination `a = 1.0, c = 1.0` follows real GDP closely (r ≈ 0.92). The documentation discusses why the other combinations drift.

## Project structure

```text
src/
├── main.c    # Experiments, simulation step, Pearson correlation
├── file.c    # CSV parsing, quarterly → annual aggregation, data loading
├── file.h
└── data.h    # YearData structure
data/         # Eurostat input data
results/      # Simulation output (generated)
```

## Author

Daria Kinash
