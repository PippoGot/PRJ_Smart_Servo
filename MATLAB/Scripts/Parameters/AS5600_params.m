%% AS5600 Magnetic Rotaty Encoder (from datasheet and design choices)

AS5600.fs = 2*pi;   % full scale angle                                      [rad]
AS5600.bits = 12;   % number of bits of measure                             [#]

AS5600.delta_th = AS5600.fs / (2^AS5600.bits);  % quantization step         [rad]