%% INA219 Current Sensor (from datasheet and design choices)
tmp = struct;

% Circuit parameters
INA219.Imax = 2.5;      % maximum expected current                          [A]
INA219.Rs = 0.01;       % shunt resistor                                    [Ohm]
INA219.Cs = 10e-8;      % filtering capacitor                               [pF]

% Device and register addresses
INA219.DEV1_ADDR = 0x40;        % I2C address of device 1                   [uint8]
INA219.DEV2_ADDR = 0x45;        % I2C address of device 2                   [uint8]

INA219.I_ADDR = 0x04;           % address of current register               [uint8]
INA219.BusV_ADDR = 0x02;        % address of bus voltage register           [uint8]
INA219.Cal_ADDR = 0x05;         % address of calibration register           [uint8]

% Calibration and LSB parameters
INA219.I_LSB = INA219.Imax / 2^15;    % current quantization step           [A]
INA219.V_LSB = 4e-3;                  % voltage quantization step           [V]

tmp.cal = round(0.04096/(INA219.I_LSB*INA219.Rs));
INA219.Cal = tmp.cal;           % calibration register value                [uint16]

% Measurement noise parameters
INA219.I_PSD = 2.6067e-9;   % power spectral density of current             [W/Hz]
INA219.V_PSD = 1.4399e-9;   % power spectral density of voltage             [W/Hz]

% To obtain PSDs: (is the correct way? it works good enough)
% array_of_data;
% PSD = mean(pwelch(array_of_data)) * sampling_time;

clear tmp;