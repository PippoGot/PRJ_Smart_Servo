function [x_dot, Va, T] = motor_linear(x, u, motor_model)
% Implements linear DC motor model

x_dot = motor_model.F*x + motor_model.G*u;

Va = motor_model.Vcc*u;

Tm = motor_model.Kphi*x(3);
Tf = motor_model.Be*x(2);

T = [Tm, Tf];

end

