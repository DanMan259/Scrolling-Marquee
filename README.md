# Scrolling Marquee

An embedded system implementation of a scrolling marquee. Developed for the Altera DE0 board using C and Assembly (Nios II instruction set). Messages are displayed across the four seven-segment display's of the DE0 board. This project was developed using specifications from an embedded systems and microprocessor interfacing course - Queen's University (ELEC371).

## Options

### Scroll Speed

The scroll speed can be adjusted using the following combinations of switches 0 and 1

| Switch 0 | Switch 1 | Scroll Speed (seconds) |
|:--------:|:--------:|:----------------------:|
|     1    |     1    |          0.25          |
|     1    |     0    |           0.5          |
|     0    |     1    |            1           |
|     0    |     0    |            2           |

### Pause Mode

Button 1 can be used to toggle pause mode. In pause mode, scrolling is stopped.

## Contributors

* Elikem Hermon [@eliherm](https://github.com/eliherm)
* Daniyal Maniar [@DanMan259](https://github.com/DanMan259)
* Hermann Krohn [@HermannKrohn](https://github.com/HermannKrohn)

## License

* [MIT](LICENSE)