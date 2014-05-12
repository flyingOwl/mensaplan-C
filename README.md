# mensaplan-C

This C application features a simple yet powerful way to access the online mealplans of Studentenwerk-Berlin-powered canteens.

## Installation

Just clone this repository (or download it in any way). Inside the code directory simply call ``` make ```.
There is also `make release` to run gcc with the -O2 switch.

You will get an executable called `Mensaplan-C`

## Usage

On the very first start the applications implicitly uses the `--init` switch. This will show a list of available canteens.
To save this selection a config file is written to your home directory.

For the next runs the application reads this file and loads the correct mealplan

## Options

For a list of all available switches just call
```./Mensaplan-C --help```

Have fun and enjoy your meal!
