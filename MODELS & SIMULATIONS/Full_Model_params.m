%% Initialization

clear
close all 
clc


%% Power Parameters

pwr.Vcc = 5;        % supply voltage of motor and power components          [V]
pwr.Vuc = 3.3;      % supply voltage of microcontroller                     [V]


%% H-Bridge Parameters

% PMOS
PMOS.Vth = -0.7;        % threshold voltage of the P-MOSFETs                [V]
PMOS.Rdson = 0.09;      % resistance of the MOSFETs channel when on         [ohm]
PMOS.Vgson = -4.5;      % gate-source voltage for Rdson                     [V]
PMOS.Idson = -2.2;      % drain-source current for Rdson                    [A]

PMOS.Ciss = 610;        % input capacitance                                 [pF]
PMOS.Crss = 170;        % reverse transfer capacitance                      [pF]
PMOS.Coss = 310;        % output capacitance                                [pF]

% NMOS
NMOS.Vth = 0.7;         % threshold voltage of the N-MOSFETs                [V]
NMOS.Rdson = 0.05;      % resistance of the MOSFETs channel when on         [ohm]
NMOS.Vgson = 4.5;       % gate-source voltage for Rdson                     [V]
NMOS.Idson = 2.6;       % drain-source current for Rdson                    [A]

NMOS.Ciss = 660;        % input capacitance                                 [pF]
NMOS.Crss = 140;        % reverse transfer capacitance                      [pF]
NMOS.Coss = 280;        % output capacitance                                [pF]

% Body Diode
diode.Vd = 1;           % diode forward voltage                             [V]
diode.Id = 2.5;         % diode source current                              [A]


%% Inverter MOSFET Parameters

AO3400.Vth = 1.4;       % threshold voltage of the MOSFET                   [V]
AO3400.Rdson = 0.04;    % resistance of the MOSFET channel when on          [ohm]
AO3400.Vgson = 4.5;     % gate-source voltage for Rdson                     [V]
AO3400.Idson = 5;       % drain-source current for Rdson                    [A]

AO3400.Ciss = 1050;     % input capacitance                                 [pF]
AO3400.Crss = 77;       % reverse transfer capacitance                      [pF]
AO3400.Coss = 99;       % output capacitance                                [pF]


%% Motor Parameters

motor.Ra = 2.4;                 % motor armature resistance                 [ohm]
motor.La = 0;                   % motor armature inductance [H]             TO MEASURE

motor.wmo = 12000;              % motor no-load speed                       [rpm]
motor.Imo = 0.14;               % motor no-load current                     [A]

% rated voltage = 5V, 10% tolerances
motor.wN = 10600;               % motor rated speed                         [rpm]
motor.lN = 0.98e-3;             % motor rated load                          [Nm]

motor.gearbox = 11/(61*36);     % gearbox ratio                             [#]





















