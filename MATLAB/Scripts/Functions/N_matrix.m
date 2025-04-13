function Nn = N_matrix(x, motor_model)
% Non-Linear motor model components

w_m = x(2) / motor_model.Kg;
z = x(4);

Zn = motor_model.s0 * abs(w_m) * z / stribeck(w_m, motor_model);

n_2 = motor_model.Kg * motor_model.s1 / motor_model.Je;

Nn = [0; n_2 * Zn; 0; -Zn];

end

