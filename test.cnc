;Basic magazine g-code for USC conversion to UMP footprint
;copyright 2017 Abbott Analytical Products all rights reserved.
;with offsets set to trial run for sim mode. See G55 note.


G40	; disable tool radius compensation
G49	; disable tool length compensation
G80	; cancel modal motion

; set 54 axis
G54	; select coordinate system 1
G10 L2 P1 X0 Y0 Z0; 170618 zero
; set set axis for G55 coordinates. 
; This is work materil's location placement 
; within PCNC 770 work envelope
G55	; select coordinate system 1
G10 L2 P1 X .1552 Y -4.126 Z -11.97  ;170610 offset


G90	; disable incremental moves
G20	; force empiral over metric
F300	; set feedrate
S1000	; set spindle speed
G61	; exact path mode
M3	; start spindle
G04 P3.0	; wait for 3.0 seconds
G0 X0.283465 Y0.267717 Z1.000000
F100	; set feedrate
G1 Z0.039764
F300	; set feedrate
G1 X1.771653
  Y0.370079
  X0.283465
  Y0.472441
  X1.771653
  Y0.574803
  X0.283465
  Y0.677165
  X1.771653
  Y0.779527
  X0.283465
  Y0.881890
  X1.771653
  Y0.984252
  X0.283465
  Y1.086614
  X1.771653
  Y1.188976
  X0.283465
  Y1.291338
  X1.771653
F100	; set feedrate
G1 Z0.000000
F300	; set feedrate
G1 X0.283465
  Y1.188976
  X1.771653
  Y1.086614
  X0.283465
  Y0.984252
  X1.771653
  Y0.881890
  X0.283465
  Y0.779527
  X1.771653
  Y0.677165
  X0.283465
  Y0.574803
  X1.771653
  Y0.472441
  X0.283465
  Y0.370079
  X1.771653
  Y0.267717
  X0.283465
M5	; stop spindle
G0 Z1.000000
F300	; set feedrate
S1000	; set spindle speed
G61	; exact path mode
M3	; start spindle
G04 P3.0	; wait for 3.0 seconds
G0 X1.771653 Y0.425197
F100	; set feedrate
G1 Z0.039370
F300	; set feedrate
G1 X2.196850
  Y0.533183
  X1.771653
  Y0.641170
  X2.196850
  Y0.749156
  X1.771653
  Y0.857143
  X2.196850
  Y0.965129
  X1.771653
  Y1.073116
  X2.196850
  Y1.181102
  X1.771653
F100	; set feedrate
G1 Z0.000000
F300	; set feedrate
G1 X2.196850
  Y1.073116
  X1.771653
  Y0.965129
  X2.196850
  Y0.857143
  X1.771653
  Y0.749156
  X2.196850
  Y0.641170
  X1.771653
  Y0.533183
  X2.196850
  Y0.425197
  X1.771653
M5	; stop spindle
G0 Z1.000000
F300	; set feedrate
S1000	; set spindle speed
G61	; exact path mode
M3	; start spindle
G04 P3.0	; wait for 3.0 seconds
G0 X2.196850 Y0.818898
F100	; set feedrate
G1 Z0.039764
F300	; set feedrate
G1 X2.480315
  Y0.913386
  X2.196850
  Y1.007874
  X2.480315
  Y1.102362
  X2.196850
  Y1.196850
  X2.480315
F100	; set feedrate
G1 Z0.000000
F300	; set feedrate
G1 X2.196850
  Y1.102362
  X2.480315
  Y1.007874
  X2.196850
  Y0.913386
  X2.480315
  Y0.818898
  X2.196850
M5	; stop spindle
G0 Z1.000000
M2	; end program

