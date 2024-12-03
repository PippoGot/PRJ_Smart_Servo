%% IRF7307 H-Bridge (from datasheet)

% PMOS
bridge.PMOS.Vth = -0.7;     % threshold voltage of the MOSFET               [V]

bridge.PMOS.Rdson = 0.09;   % resistance of MOSFET channel                  [Ohm]
bridge.PMOS.Vgson = -4.5;   % gate-source voltage for Rdson                 [V]
bridge.PMOS.Idson = -2.2;   % drain-source current for Rdson                [A]   

bridge.PMOS.Ciss = 610;     % input capacitance                             [pF]
bridge.PMOS.Crss = 170;     % reverse transfer capacitance                  [pF]
bridge.PMOS.Coss = 310;     % output capacitance                            [pF]

% NMOS
bridge.NMOS.Vth = 0.7;      % threshold voltage of the MOSFET               [V]

bridge.NMOS.Rdson = 0.05;   % resistance of MOSFET channel                  [Ohm]
bridge.NMOS.Vgson = 4.5;    % gate-source voltage for Rdson                 [V]
bridge.NMOS.Idson = 2.6;    % drain-source current for Rdson                [A]   

bridge.NMOS.Ciss = 660;     % input capacitance                             [pF]
bridge.NMOS.Crss = 140;     % reverse transfer capacitance                  [pF]
bridge.NMOS.Coss = 280;     % output capacitance                            [pF]

% Body Diode
bridge.Diode.Vd = 1;        % diode forward voltage                         [V]
bridge.Diode.Id = 2.5;      % diode source current                          [A]

% Passive Components
bridge.Rp = 10e3;           % pull-up/pull-down resistors                   [Ohm]
bridge.Rg = 100;            % gate resistors                                [Ohm]


