%% DRV8830 DC Motor Driver (from datasheet)

% Voltages levels
DRV8830.V_ref = 1.285;                  % internal reference voltage        [V]
DRV8830.V_step = DRV8830.V_ref / 16;    % quantization step                 [V]
DRV8830.V_min = DRV8830.V_step * 6;     % lowest nonzero voltage            [V]
DRV8830.V_max = DRV8830.V_step * 63;    % highest nonzero voltage           [V]


% Current limiting parameters
DRV8830.R_sens = 0.1;                   % current sensing resistor          [Ohm]
DRV8830.I_limit = 0.2 / DRV8830.R_sens; % absolute maximum current          [A]


% to refine the model should include PWM effect, current limiting effects
% and shutdown effects. evaluate if necessary.


%% INA219 Current and Voltage Sensor (from datasheet and design choices)

% Model parameters
INA219.Imax = 1;                        % maximum expected current          [A]
INA219.Rs = 0.1;                        % shunt resistor                    [Ohm]
INA219.Cs = 1e-7;                       % filtering capacitor               [F]

INA219.Istep = INA219.Imax / 2^15;      % current quantization step         [A]
% INA219.Istep = 250e-6;
INA219.Vstep = 4e-3;                    % voltage quantization step         [V]

% measure noise for voltage and current
INA219.Ipsd = 2.6067e-9;                % power spectral density of current [W/Hz]
INA219.Vpsd = 1.4399e-9;                % power spectral density of voltage [W/Hz]
% To obtain PSDs: (is the correct way? it works good enough)
% array_of_data;
% PSD = mean(pwelch(array_of_data)) * sampling_time;

% Device and register addresses
INA219.DEV1_ADDR = 0x40;                % I2C address of device 1           [uint8]
INA219.DEV2_ADDR = 0x45;                % I2C address of device 2           [uint8]

INA219.I_ADDR = 0x04;                   % address of current register       [uint8]
INA219.BV_ADDR = 0x02;                  % address of bus voltage register   [uint8]
INA219.CAL_ADDR = 0x05;                 % address of calibration register   [uint8]


% Calibration register value
cal = round(0.04096/(INA219.Istep*INA219.Rs));
INA219.cal = cal*2;                       % calibration register value        [uint16]
clear cal


%% AS5600 Magnetic Rotary Encoder (from datasheet and design choices)

% Model parameters
AS5600.FS = 2*pi;                       % full scale angle                  [rad]
AS5600.Astep = AS5600.FS/(4096);        % quantization step                 [rad]


% Device and register addresses
AS5600.DEV_ADDR = 0x36;                 % device I2C address                [uint8]
AS5600.RANGLE_ADDR = 0x0C;              % raw angle register address        [uint8]
AS5600.ANGLE_ADDR = 0x0E;               % angle register address            [uint8]


