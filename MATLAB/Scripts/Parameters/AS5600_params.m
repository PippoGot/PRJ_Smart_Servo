%% AS5600 Magnetic Rotaty Encoder (from datasheet and design choices)

% Quantization parameters
AS5600.bits = 12;                           % number of bits of measure     [#]
AS5600.FS = 2*pi;                           % full scale angle              [rad]
AS5600.q_step = AS5600.FS/(2^AS5600.bits);  % quantization step             [rad]


% Device and Register addresses
AS5600.DEV_ADDR = 0x36;     % device I2C address                            [uint8]
AS5600.TH_ADDR = 0x0E;      % angle register address                        [uint8]