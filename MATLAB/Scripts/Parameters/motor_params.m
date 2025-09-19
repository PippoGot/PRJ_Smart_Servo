%% Motor Model Parameters (from measurements and estimations)

% Motor Linear Model Parameters

% motor.Ra = 2.2;                                 % armature resistance       [Ohm]
motor.Ra = 3.2485;
motor.La = 7e-3;                                % armature inductance       [H]

% motor.Kphi = 1.8e-3;                            % BEMF constant             [V*s]
motor.Kphi = 1e-3;
motor.Kphi_0 = 8.4134e-4;
motor.Wo = 1.4862e-3;
motor.E = sqrt(1e-2);

% motor.Je = 1.0624e-8;                           % total inertia             [Kg*m^2]
motor.Je = 1.4669e-9;
% motor.Be = 1.4949e-8;                           % total viscous friction    [N*m*s]
motor.Be = 1.0546e-8;

motor.Kg = 11/(61*36);                          % gearbox (motor to shaft)  [#]

% LuGre Friction Model Parameters
% motor.Tc = 7.5179e-5;                           % coulomb friction torque   [N*m]
motor.Tc = 9.3828e-6;

motor.Tsb = 2*motor.Tc;                         % breakaway torque          [N*m]
motor.Wsb = 0.1;                               % breakaway speed           [rad/s]

motor.Xi = 0.5;                                 % damping coefficient
motor.s0 = 1;                                   % bristle stiffness
motor.s1 = 2*motor.Xi*sqrt(motor.s0*motor.Je);  % bristle damping

