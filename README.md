# pso

## Equation

### Position in next iteration

<img src="https://latex.codecogs.com/png.latex?\dpi{200}&space;\overrightarrow{{X_{i}}^{t&plus;1}}&space;=&space;\overrightarrow{{X_{i}}^{t}}&space;&plus;&space;\overrightarrow{{V_{i}}^{t&plus;1}}" title="\overrightarrow{{X_{i}}^{t+1}} = \overrightarrow{{X_{i}}^{t}} + \overrightarrow{{V_{i}}^{t+1}}" />

### Velocity in next iteration

<img src="https://latex.codecogs.com/png.latex?\dpi{200}&space;\overrightarrow{{V_{i}}^{t&plus;1}}&space;=&space;\mathrm{w}\overrightarrow{{V_{i}}^{t}}&space;&plus;&space;\mathrm{c_{1}r_{1}}(\overrightarrow{{P_{i}}^{t}}&space;-&space;\overrightarrow{{X_{i}}^{t}})&space;&plus;&space;\mathrm{c_{2}r_{2}}(\overrightarrow{G^{t}}&space;-&space;\overrightarrow{{X_{i}}^{t}})" title="\overrightarrow{{V_{i}}^{t+1}} = \mathrm{w}\overrightarrow{{V_{i}}^{t}} + \mathrm{c_{1}r_{1}}(\overrightarrow{{P_{i}}^{t}} - \overrightarrow{{X_{i}}^{t}}) + \mathrm{c_{2}r_{2}}(\overrightarrow{G^{t}} - \overrightarrow{{X_{i}}^{t}})" />

where

`r1` and `r2` = random constant between [0, 1]
