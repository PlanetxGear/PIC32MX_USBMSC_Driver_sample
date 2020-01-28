EESchema Schematic File Version 4
LIBS:PIC32MX_USBMSC_Driver-cache
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "PIC32MX_USBMSC_DRIVER"
Date "2020-01-28"
Rev "1"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L mylib20161223:S-812C U1
U 1 1 5E2E42E5
P 2200 6250
F 0 "U1" H 2200 6657 50  0000 C CNN
F 1 "S-812C" H 2200 6566 50  0000 C CNN
F 2 "TO92-GIO" H 2200 6475 50  0000 C CNN
F 3 "" H 2200 6250 50  0000 C CNN
	1    2200 6250
	1    0    0    -1  
$EndComp
$Comp
L mylib20161223:C C1
U 1 1 5E2E5077
P 1600 6350
F 0 "C1" H 1715 6391 43  0000 L CNN
F 1 "0.1uF" H 1715 6310 43  0000 L CNN
F 2 "" H 1638 6200 30  0000 C CNN
F 3 "" H 1600 6350 60  0000 C CNN
	1    1600 6350
	1    0    0    -1  
$EndComp
$Comp
L mylib20161223:CP C2
U 1 1 5E2E581B
P 2850 6350
F 0 "C2" H 2968 6396 50  0000 L CNN
F 1 "10uF" H 2968 6305 50  0000 L CNN
F 2 "" H 2888 6200 50  0000 C CNN
F 3 "" H 2850 6350 50  0000 C CNN
	1    2850 6350
	1    0    0    -1  
$EndComp
$Comp
L mylib20161223:+5.0V #PWR01
U 1 1 5E2E6728
P 1500 6150
F 0 "#PWR01" H 1500 6075 30  0001 C CNN
F 1 "+5.0V" H 1500 6288 30  0000 C CNN
F 2 "" H 1500 6150 60  0000 C CNN
F 3 "" H 1500 6150 60  0000 C CNN
	1    1500 6150
	1    0    0    -1  
$EndComp
$Comp
L mylib20161223:+3.3V #PWR04
U 1 1 5E2E6F3A
P 3000 6150
F 0 "#PWR04" H 3000 6075 30  0001 C CNN
F 1 "+3.3V" H 3000 6288 30  0000 C CNN
F 2 "" H 3000 6150 60  0000 C CNN
F 3 "" H 3000 6150 60  0000 C CNN
	1    3000 6150
	1    0    0    -1  
$EndComp
$Comp
L mylib20161223:GND #PWR02
U 1 1 5E2E76DB
P 2200 6650
F 0 "#PWR02" H 2200 6650 30  0001 C CNN
F 1 "GND" H 2200 6580 30  0001 C CNN
F 2 "" H 2200 6650 60  0000 C CNN
F 3 "" H 2200 6650 60  0000 C CNN
	1    2200 6650
	1    0    0    -1  
$EndComp
Wire Wire Line
	1500 6150 1500 6200
Wire Wire Line
	1500 6200 1600 6200
Wire Wire Line
	1800 6200 1600 6200
Connection ~ 1600 6200
Wire Wire Line
	2600 6200 2850 6200
Wire Wire Line
	2850 6200 3000 6200
Wire Wire Line
	3000 6200 3000 6150
Connection ~ 2850 6200
Wire Wire Line
	1600 6500 2200 6500
Wire Wire Line
	2200 6500 2850 6500
Connection ~ 2200 6500
Wire Wire Line
	2200 6500 2200 6650
$Comp
L mylib20161223:PIC32MX250F128B U2
U 1 1 5E2E925C
P 5900 3500
F 0 "U2" H 5900 5193 60  0000 C CNN
F 1 "PIC32MX250F128B" H 5900 5087 60  0000 C CNN
F 2 "" H 5900 3900 60  0000 C CNN
F 3 "www.microchip.com" H 5900 4981 60  0000 C CNN
	1    5900 3500
	1    0    0    -1  
$EndComp
$Comp
L mylib20161223:+3.3V #PWR013
U 1 1 5E2EDCEF
P 7600 2200
F 0 "#PWR013" H 7600 2125 30  0001 C CNN
F 1 "+3.3V" H 7600 2338 30  0000 C CNN
F 2 "" H 7600 2200 60  0000 C CNN
F 3 "" H 7600 2200 60  0000 C CNN
	1    7600 2200
	1    0    0    -1  
$EndComp
$Comp
L mylib20161223:+3.3V #PWR015
U 1 1 5E2EE434
P 7600 3500
F 0 "#PWR015" H 7600 3425 30  0001 C CNN
F 1 "+3.3V" H 7600 3638 30  0000 C CNN
F 2 "" H 7600 3500 60  0000 C CNN
F 3 "" H 7600 3500 60  0000 C CNN
	1    7600 3500
	1    0    0    -1  
$EndComp
$Comp
L mylib20161223:+3.3V #PWR011
U 1 1 5E2EE829
P 4200 4500
F 0 "#PWR011" H 4200 4425 30  0001 C CNN
F 1 "+3.3V" H 4200 4638 30  0000 C CNN
F 2 "" H 4200 4500 60  0000 C CNN
F 3 "" H 4200 4500 60  0000 C CNN
	1    4200 4500
	1    0    0    -1  
$EndComp
$Comp
L mylib20161223:GND #PWR014
U 1 1 5E2EE9B8
P 7600 2550
F 0 "#PWR014" H 7600 2550 30  0001 C CNN
F 1 "GND" H 7600 2480 30  0001 C CNN
F 2 "" H 7600 2550 60  0000 C CNN
F 3 "" H 7600 2550 60  0000 C CNN
	1    7600 2550
	1    0    0    -1  
$EndComp
$Comp
L mylib20161223:GND #PWR016
U 1 1 5E2EF102
P 7600 4150
F 0 "#PWR016" H 7600 4150 30  0001 C CNN
F 1 "GND" H 7600 4080 30  0001 C CNN
F 2 "" H 7600 4150 60  0000 C CNN
F 3 "" H 7600 4150 60  0000 C CNN
	1    7600 4150
	1    0    0    -1  
$EndComp
$Comp
L mylib20161223:C C3
U 1 1 5E2EF3A6
P 3950 4500
F 0 "C3" V 3713 4500 43  0000 C CNN
F 1 "0.1uF" V 3794 4500 43  0000 C CNN
F 2 "" H 3988 4350 30  0000 C CNN
F 3 "" H 3950 4500 60  0000 C CNN
	1    3950 4500
	0    1    1    0   
$EndComp
$Comp
L mylib20161223:C C4
U 1 1 5E2F050C
P 7600 2350
F 0 "C4" H 7715 2391 43  0000 L CNN
F 1 "0.1uF" H 7715 2310 43  0000 L CNN
F 2 "" H 7638 2200 30  0000 C CNN
F 3 "" H 7600 2350 60  0000 C CNN
	1    7600 2350
	1    0    0    -1  
$EndComp
$Comp
L mylib20161223:CP C6
U 1 1 5E2F104C
P 7850 4000
F 0 "C6" V 8000 4000 50  0000 C CNN
F 1 "10uF" V 7686 4000 50  0000 C CNN
F 2 "" H 7888 3850 50  0000 C CNN
F 3 "" H 7850 4000 50  0000 C CNN
	1    7850 4000
	0    1    1    0   
$EndComp
$Comp
L mylib20161223:GND #PWR018
U 1 1 5E2F19E2
P 8100 3550
F 0 "#PWR018" H 8100 3550 30  0001 C CNN
F 1 "GND" H 8100 3480 30  0001 C CNN
F 2 "" H 8100 3550 60  0000 C CNN
F 3 "" H 8100 3550 60  0000 C CNN
	1    8100 3550
	1    0    0    -1  
$EndComp
$Comp
L mylib20161223:GND #PWR010
U 1 1 5E2F20B0
P 4200 3550
F 0 "#PWR010" H 4200 3550 30  0001 C CNN
F 1 "GND" H 4200 3480 30  0001 C CNN
F 2 "" H 4200 3550 60  0000 C CNN
F 3 "" H 4200 3550 60  0000 C CNN
	1    4200 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	4300 3500 4200 3500
Wire Wire Line
	4200 3500 4200 3550
Wire Wire Line
	4100 4500 4200 4500
Wire Wire Line
	4300 4500 4200 4500
Connection ~ 4200 4500
$Comp
L mylib20161223:GND #PWR08
U 1 1 5E2F2E4B
P 3700 4550
F 0 "#PWR08" H 3700 4550 30  0001 C CNN
F 1 "GND" H 3700 4480 30  0001 C CNN
F 2 "" H 3700 4550 60  0000 C CNN
F 3 "" H 3700 4550 60  0000 C CNN
	1    3700 4550
	1    0    0    -1  
$EndComp
Wire Wire Line
	3800 4500 3700 4500
Wire Wire Line
	3700 4500 3700 4550
Wire Wire Line
	7500 2200 7600 2200
Connection ~ 7600 2200
Wire Wire Line
	7500 2500 7600 2500
Wire Wire Line
	7600 2500 7600 2550
Connection ~ 7600 2500
Wire Wire Line
	7500 4100 7600 4100
Wire Wire Line
	7600 4100 7600 4150
$Comp
L mylib20161223:R R1
U 1 1 5E2F4E44
P 4200 1950
F 0 "R1" H 4271 1988 40  0000 L CNN
F 1 "10K" H 4271 1912 40  0000 L CNN
F 2 "" V 4130 1950 30  0000 C CNN
F 3 "" H 4200 1950 30  0000 C CNN
	1    4200 1950
	1    0    0    -1  
$EndComp
$Comp
L mylib20161223:+3.3V #PWR09
U 1 1 5E2F5822
P 4200 1650
F 0 "#PWR09" H 4200 1575 30  0001 C CNN
F 1 "+3.3V" H 4200 1788 30  0000 C CNN
F 2 "" H 4200 1650 60  0000 C CNN
F 3 "" H 4200 1650 60  0000 C CNN
	1    4200 1650
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 2200 4300 2200
Wire Wire Line
	4200 1700 4200 1650
$Comp
L mylib20161223:CN-ICSP CN1
U 1 1 5E2F66BE
P 2900 2500
F 0 "CN1" H 2808 3087 60  0000 C CNN
F 1 "CN-ICSP" H 2808 2981 60  0000 C CNN
F 2 "" H 2900 2350 60  0000 C CNN
F 3 "" H 2900 2350 60  0000 C CNN
	1    2900 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	3000 2800 4300 2800
Wire Wire Line
	4300 2700 3000 2700
Wire Wire Line
	3000 2700 3000 2650
Wire Wire Line
	3000 2200 4000 2200
Connection ~ 4200 2200
$Comp
L mylib20161223:+3.3V #PWR05
U 1 1 5E2F8B27
P 3100 2350
F 0 "#PWR05" H 3100 2275 30  0001 C CNN
F 1 "+3.3V" H 3100 2488 30  0000 C CNN
F 2 "" H 3100 2350 60  0000 C CNN
F 3 "" H 3100 2350 60  0000 C CNN
	1    3100 2350
	1    0    0    -1  
$EndComp
$Comp
L mylib20161223:GND #PWR06
U 1 1 5E2F9362
P 3100 2550
F 0 "#PWR06" H 3100 2550 30  0001 C CNN
F 1 "GND" H 3100 2480 30  0001 C CNN
F 2 "" H 3100 2550 60  0000 C CNN
F 3 "" H 3100 2550 60  0000 C CNN
	1    3100 2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	3000 2350 3100 2350
Wire Wire Line
	3000 2500 3100 2500
Wire Wire Line
	3100 2500 3100 2550
$Comp
L mylib20161223:CN-UART-401 CN2
U 1 1 5E2FA560
P 9050 4450
F 0 "CN2" H 8722 4472 60  0000 R CNN
F 1 "CN-UART-401" H 8722 4578 60  0000 R CNN
F 2 "" H 9050 4300 60  0000 C CNN
F 3 "" H 9050 4300 60  0000 C CNN
	1    9050 4450
	-1   0    0    1   
$EndComp
Wire Wire Line
	7500 4300 8950 4300
Wire Wire Line
	7500 4400 8950 4400
Wire Wire Line
	8950 4400 8950 4450
$Comp
L mylib20161223:GND #PWR020
U 1 1 5E2FC627
P 8650 4900
F 0 "#PWR020" H 8650 4900 30  0001 C CNN
F 1 "GND" H 8650 4830 30  0001 C CNN
F 2 "" H 8650 4900 60  0000 C CNN
F 3 "" H 8650 4900 60  0000 C CNN
	1    8650 4900
	1    0    0    -1  
$EndComp
$Comp
L mylib20161223:+5.0V #PWR023
U 1 1 5E2FD0AB
P 8850 4900
F 0 "#PWR023" H 8850 4825 30  0001 C CNN
F 1 "+5.0V" H 8850 5038 30  0000 C CNN
F 2 "" H 8850 4900 60  0000 C CNN
F 3 "" H 8850 4900 60  0000 C CNN
	1    8850 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	8950 4600 8650 4600
Wire Wire Line
	8650 4600 8650 4900
Wire Wire Line
	8950 4750 8950 4900
Wire Wire Line
	8950 4900 8850 4900
$Comp
L mylib20161223:LED D1
U 1 1 5E2FFBE9
P 4300 5650
F 0 "D1" V 4254 5728 50  0000 L CNN
F 1 "LED" V 4345 5728 50  0000 L CNN
F 2 "" H 4300 5650 60  0000 C CNN
F 3 "" H 4300 5650 60  0000 C CNN
	1    4300 5650
	0    1    1    0   
$EndComp
$Comp
L mylib20161223:GND #PWR012
U 1 1 5E30084D
P 4300 5950
F 0 "#PWR012" H 4300 5950 30  0001 C CNN
F 1 "GND" H 4300 5880 30  0001 C CNN
F 2 "" H 4300 5950 60  0000 C CNN
F 3 "" H 4300 5950 60  0000 C CNN
	1    4300 5950
	1    0    0    -1  
$EndComp
Wire Wire Line
	4300 4700 4300 4850
Wire Wire Line
	4300 5350 4300 5450
Wire Wire Line
	4300 5850 4300 5950
$Comp
L mylib20161223:Resonator_Small Y1
U 1 1 5E303290
P 2800 3800
F 0 "Y1" V 2495 3750 50  0000 C CNN
F 1 "4Mhz" V 2586 3750 50  0000 C CNN
F 2 "" H 2775 3800 50  0001 C CNN
F 3 "~" H 2775 3800 50  0001 C CNN
	1    2800 3800
	0    1    1    0   
$EndComp
Wire Wire Line
	3000 3800 3000 3900
Wire Wire Line
	3000 3900 2900 3900
$Comp
L mylib20161223:GND #PWR03
U 1 1 5E3067AC
P 2450 3900
F 0 "#PWR03" H 2450 3900 30  0001 C CNN
F 1 "GND" H 2450 3830 30  0001 C CNN
F 2 "" H 2450 3900 60  0000 C CNN
F 3 "" H 2450 3900 60  0000 C CNN
	1    2450 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	2450 3900 2450 3800
Wire Wire Line
	2450 3800 2600 3800
Wire Wire Line
	2900 3700 4300 3700
Wire Wire Line
	3000 3800 4300 3800
$Comp
L mylib20161223:SW_PUSH SW1
U 1 1 5E30A60A
P 3700 1950
F 0 "SW1" H 3700 2205 50  0000 C CNN
F 1 "SW_PUSH" H 3700 2114 50  0000 C CNN
F 2 "" H 3700 1950 60  0000 C CNN
F 3 "" H 3700 1950 60  0000 C CNN
	1    3700 1950
	1    0    0    -1  
$EndComp
$Comp
L mylib20161223:GND #PWR07
U 1 1 5E30B946
P 3400 2050
F 0 "#PWR07" H 3400 2050 30  0001 C CNN
F 1 "GND" H 3400 1980 30  0001 C CNN
F 2 "" H 3400 2050 60  0000 C CNN
F 3 "" H 3400 2050 60  0000 C CNN
	1    3400 2050
	1    0    0    -1  
$EndComp
Wire Wire Line
	3400 1950 3400 2050
Wire Wire Line
	4000 1950 4000 2200
Connection ~ 4000 2200
Wire Wire Line
	4000 2200 4200 2200
$Comp
L mylib20161223:CN_USB_A P1
U 1 1 5E30DBF1
P 9100 3800
F 0 "P1" V 9082 4088 50  0000 L CNN
F 1 "CN_USB_A" V 9173 4088 50  0000 L CNN
F 2 "" V 9050 3700 50  0000 C CNN
F 3 "" V 9050 3700 50  0000 C CNN
	1    9100 3800
	0    1    1    0   
$EndComp
Wire Wire Line
	7500 3800 8800 3800
Wire Wire Line
	8800 3700 7500 3700
Wire Wire Line
	7500 3500 7600 3500
Wire Wire Line
	7600 3500 7700 3500
Connection ~ 7600 3500
Wire Wire Line
	8000 3500 8100 3500
Wire Wire Line
	8100 3500 8100 3550
$Comp
L mylib20161223:+5.0V #PWR021
U 1 1 5E318C4C
P 8700 3600
F 0 "#PWR021" H 8700 3525 30  0001 C CNN
F 1 "+5.0V" H 8700 3738 30  0000 C CNN
F 2 "" H 8700 3600 60  0000 C CNN
F 3 "" H 8700 3600 60  0000 C CNN
	1    8700 3600
	1    0    0    -1  
$EndComp
$Comp
L mylib20161223:GND #PWR022
U 1 1 5E31962E
P 8700 3950
F 0 "#PWR022" H 8700 3950 30  0001 C CNN
F 1 "GND" H 8700 3880 30  0001 C CNN
F 2 "" H 8700 3950 60  0000 C CNN
F 3 "" H 8700 3950 60  0000 C CNN
	1    8700 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	8800 3900 8700 3900
Wire Wire Line
	8700 3900 8700 3950
Wire Wire Line
	8800 3600 8700 3600
$Comp
L mylib20161223:+5.0V #PWR017
U 1 1 5E31BE6A
P 7600 4700
F 0 "#PWR017" H 7600 4625 30  0001 C CNN
F 1 "+5.0V" H 7600 4838 30  0000 C CNN
F 2 "" H 7600 4700 60  0000 C CNN
F 3 "" H 7600 4700 60  0000 C CNN
	1    7600 4700
	1    0    0    -1  
$EndComp
Wire Wire Line
	7500 4700 7600 4700
$Comp
L mylib20161223:GND #PWR019
U 1 1 5E31E15D
P 8100 4050
F 0 "#PWR019" H 8100 4050 30  0001 C CNN
F 1 "GND" H 8100 3980 30  0001 C CNN
F 2 "" H 8100 4050 60  0000 C CNN
F 3 "" H 8100 4050 60  0000 C CNN
	1    8100 4050
	1    0    0    -1  
$EndComp
$Comp
L mylib20161223:C C5
U 1 1 5E31E919
P 7850 3500
F 0 "C5" V 7613 3500 43  0000 C CNN
F 1 "0.1uF" V 7694 3500 43  0000 C CNN
F 2 "" H 7888 3350 30  0000 C CNN
F 3 "" H 7850 3500 60  0000 C CNN
	1    7850 3500
	0    1    1    0   
$EndComp
Wire Wire Line
	7500 4000 7700 4000
Wire Wire Line
	8000 4000 8100 4000
Wire Wire Line
	8100 4000 8100 4050
$Comp
L mylib20161223:R R2
U 1 1 5E2FF15E
P 4300 5100
F 0 "R2" H 4371 5138 40  0000 L CNN
F 1 "330" H 4371 5062 40  0000 L CNN
F 2 "" V 4230 5100 30  0000 C CNN
F 3 "" H 4300 5100 30  0000 C CNN
	1    4300 5100
	1    0    0    -1  
$EndComp
NoConn ~ 7500 3100
NoConn ~ 7500 3200
NoConn ~ 7500 3300
NoConn ~ 9300 4100
NoConn ~ 9200 4100
NoConn ~ 4300 2400
NoConn ~ 4300 2500
NoConn ~ 4300 2900
NoConn ~ 4300 3000
NoConn ~ 4300 4000
NoConn ~ 4300 4100
NoConn ~ 7500 4500
$EndSCHEMATC
