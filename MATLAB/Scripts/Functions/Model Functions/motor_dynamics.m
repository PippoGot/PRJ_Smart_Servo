function x_dot = motor_dynamics(x, u, motor_model, nl_options)
%MOTOR_DYNAMICS Summary of this function goes here
%   Detailed explanation goes here

%% Motor Parameters Mapping (for shorter lines)

% mechanical parameters
Be = motor_model.Be;
Je = motor_model.Je;
Kg = motor_model.Kg;

Wsb = motor_model.Wsb;
Tc = 0;
Tsb = 0;
s0 = 0;
s1 = 0;

% electrical parameters
Ra = motor_model.Ra;
La = motor_model.La;

Kphi = motor_model.Kphi_0;
E = motor_model.E;
Wo = motor_model.Wo;


%% Non-linearities Handling

% Flux Weakening Effect
if nl_options.flux == 0
    Wo = 0;
end

PHI_ws = Wo/sqrt(x(2)^2+E^2);


% Nonlinear Friction Effects
TAU_ws = 0;

if nl_options.coulomb == 1
    Tc = motor_model.Tc;
    Tsb = motor_model.Tc;
    s0 = motor_model.s0;
    s1 = motor_model.s1;
    TAU_ws = abs(x(2))/(Tc + (Tsb - Tc)*exp(-(x(2)/(Kg*Wsb))^2));
end

if nl_options.stribeck == 1
    Tsb = motor_model.Tsb;
    s0 = motor_model.s0;
    s1 = motor_model.s1;
    TAU_ws = abs(x(2))/(Tc + (Tsb - Tc)*exp(-(x(2)/(Kg*Wsb))^2));
end



%% Matrix Construction
F = zeros(4, 4);

F(1,2) = 1;

F(2,2) = -(Be + s1)/Je;
F(2,3) = Kphi*Kg/Je;
F(2,4) = -s0*Kg/Je;

F(3,2) = -Kphi/(Kg*La);
F(3,3) = -Ra/La;

F(4,2) = 1/Kg;


G = zeros(4, 1);

G(3,1) =  1/La;


N = zeros(4, 4);

% N(2, 3) = Kg/Je * PHI_ws;           % correct modeling is 0
N(2, 4) = s0*s1/Je * TAU_ws;

N(3, 2) = -1/(La*Kg) * PHI_ws;

N(4, 4) = -s0/Kg * TAU_ws;


%% Output Equation

x_dot = F*x + G*u + N*x;


end

