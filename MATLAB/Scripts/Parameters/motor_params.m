%% Motor Model Parameters (from measurements and estimations)

% Motor Linear Model Parameters
motor.Vcc = 5;                                  % motor voltage supply      [V]
motor.Ra = 2.2;                                 % armature resistance       [Ohm]
motor.La = 7e-3;                                % armature inductance       [H]

motor.Kphi = 1.8e-3;                            % BEMF constant             [V*s]

motor.Je = 1.0624e-8;                           % total inertia             [Kg*m^2]
motor.Be = 1.4949e-8;                           % total viscous friction    [N*m*s]

motor.Kg = 11/(61*36);                          % gearbox (motor to shaft)  [#]

% LuGre Friction Model Parameters
motor.Tc = 7.5179e-5;                           % coulomb friction torque   [N*m]

motor.Tsb = 2*motor.Tc;                         % breakaway torque          [N*m]
motor.w_sb = 0.1;                               % breakaway speed           [rad/s]

motor.xi = 0.1;                                 % damping coefficient
motor.s0 = 1;                                   % bristle stiffness
motor.s1 = 2*motor.xi*sqrt(motor.s0*motor.Je);  % bristle damping


%% Linear State Space Model

% F Matrix
f_22 = - motor.Be / motor.Je;
f_23 = motor.Kphi * motor.Kg / motor.Je;
f_32 = - motor.Kphi / (motor.La * motor.Kg);
f_33 = - motor.Ra / motor.La;

motor.F = [ 0,  1,       0;
            0,  f_22,    f_23;
            0,  f_32,    f_33
            ];

% G Matrix
g_3 = motor.Vcc / motor.La;

motor.G = [0; 0; g_3];

% H Matrix
motor.h1 = [1, 0, 0];
motor.h2 = [0, 0, 1];
motor.h3 = -motor.h2;

motor.H = [motor.h1; motor.h2; motor.h3];

% D Matrix
motor.D = zeros(3, 1);


%% Non-Linear State Space Model (LuGre Dynamic Friction)

% Fn Matrix
f_22 = - motor.s1 / motor.Je;
f_24 = - motor.s0 * motor.Kg / motor.Je;
f_42 = 1 / motor.Kg;

F_l = [motor.F, zeros(3, 1); zeros(1, 4)];
F_nl = [0,  0,      0,  0;
        0,  f_22,   0,  f_24;
        0,  0,      0,  0;
        0,  f_42,   0,  0
        ];

motor.Fn = F_l + F_nl;


% Gn Matrix
motor.Gn = [motor.G; 0];


%% Clear Variables

clear f_22
clear f_23
clear f_24

clear f_32
clear f_33

clear f_42

clear g_3

clear F_l
clear F_nl


