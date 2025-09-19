function J = motor_jacobian(x, motor_model, nl_options)
%MOTOR_JACOBIAN Summary of this function goes here
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


%% Motor State Mapping (for shorter lines)
th = x(1);
w = x(2);
i = x(3);
z = x(4);


%% Non-linearities Handling

% Flux Weakening Effect
if nl_options.flux == 0
    Wo = 0;
end

PHI_ws = Wo/sqrt(x(2)^2+E^2);


% Nonlinear Friction Effects
TAU_sb = 1;
TAU_ws = 0;

if nl_options.coulomb == 1
    Tc = motor_model.Tc;
    Tsb = motor_model.Tc;
    s0 = motor_model.s0;
    s1 = motor_model.s1;

    TAU_sb = Tc + (Tsb - Tc)*exp(-(w/(Kg*Wsb))^2);
    TAU_ws = abs(x(2))/TAU_sb;
end

if nl_options.stribeck == 1
    Tsb = motor_model.Tsb;
    s0 = motor_model.s0;
    s1 = motor_model.s1;

    TAU_sb = Tc + (Tsb - Tc)*exp(-(w/(Kg*Wsb))^2);
    TAU_ws = abs(x(2))/TAU_sb;
end


%% Jacobian Construction
J = zeros(4, 4);

J(1,2) = 1;

J(2,2) = -1/Je*(Be + s1 - s0*s1*sign(w)*z/TAU_sb + 2*s0*s1*w*TAU_ws*(TAU_sb - Tc)*z/(TAU_sb*(Kg*Wsb)^2));
J(2,3) = Kg*Kphi/Je;
J(2,4) = -1/Je*(s0*Kg - s0*s1*TAU_ws);

J(3,2) = -1/La*(Kphi/Kg + PHI_ws/Kg - PHI_ws*w^2/(Kg*(w^2+E^2)));
J(3,3) = -Ra/La;

J(4,2) = 1/Kg*(1 - s0*sign(w)/TAU_sb + 2*s0*w*TAU_ws*(TAU_sb - Tc)*z/(TAU_sb*(Kg*Wsb)^2));
J(4,4) = -1/Kg*(s0*TAU_ws);


end

