function Tf = stribeck(w_m, motor_model)
% Calculates the total friction torque for the given speed based on 
% Stribeck curve

Tf = motor_model.Tc + (motor_model.Tsb - motor_model.Tc) * exp(-(w_m / motor_model.w_sb)^2);

end

