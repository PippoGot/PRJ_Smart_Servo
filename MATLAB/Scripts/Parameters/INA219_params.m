%% INA219 Current Sensor (from datasheet and design choices)

INA219.DADRS1 = 0x40;   % address of device 1                               [uint8]
INA219.DADRS2 = 0x45;   % address of device 2                               [uint8]

%INA219.Current_ADRS
%INA219.Voltage_ADRS

INA219.Rs = 0.01;       % shunt resistor                                    [Ohm]
INA219.Cs = 10e-8;      % filtering capacitor                               [pF]
INA219.Imax = 2.5;      % maximum expected current                          [A]

INA219.Current_LSB = INA219.Imax / 2^16;    % current quantization step     [A]
INA219.Voltage_LSB = 4e-3;                  % voltage quantization step     [V]


INA219.Current_std = 1e-8;  % noise power of current measurements           [A^2]?
INA219.Voltage_std = 1e-4;  % noise power of voltage measurements           [V^2]?

