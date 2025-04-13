function [x_dot, Va, T] = motor_nonlinear(x, u, motor_model)
% Implements linear DC motor model with non-linear LuGre dynamic friction

x_dot = motor_model.Fn*x + motor_model.Gn*u + N_matrix(x, motor_model);

Va = motor_model.Vcc*u;

Tf = motor_model.s0*x(4) + motor_model.s1*x_dot(4) + motor_model.Be*x(2)/motor_model.Kg;
Tm = motor_model.Kphi*x(3);

T = [Tm, Tf];

end

