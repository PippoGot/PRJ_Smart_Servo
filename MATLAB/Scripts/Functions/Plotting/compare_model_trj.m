function compare_model_trj(datasim, dataset)
%UNTITLED Summary of this function goes here
%   Plot trajectories and trajectory difference for a pair of dataset.

time = datasim.time;

position = [datasim.position, dataset.position];
velocity = [datasim.velocity, dataset.velocity];
current = [datasim.current, dataset.current];
voltage = [datasim.voltage, dataset.voltage];

figure()
grid on
subplot(4, 1, 1)
grid on
plot(time, position)
title('Position [rad]')

subplot(4, 1, 2)
grid on
plot(time, velocity)
title('Velocity [rad/s]')

subplot(4, 1, 3)
grid on
plot(time, current)
title('Current [A]')

subplot(4, 1, 4)
grid on
plot(time, voltage)
title('Voltage [V]')

%%

figure()
grid on
subplot(4, 1, 1)
grid on
plot(time, (sgolayfilt(position(:, 1)-position(:, 2), 5, 501)))
title('Absolute Position Error [rad]')

subplot(4, 1, 2)
grid on
plot(time, (sgolayfilt(velocity(:, 1)-velocity(:, 2), 5, 501)))
title('Absolute Velocity Error [rad/s]')

subplot(4, 1, 3)
grid on
plot(time, (sgolayfilt(current(:, 1)-current(:, 2), 5, 501)))
title('Absolute Current Error [A]')

subplot(4, 1, 4)
grid on
plot(time, (sgolayfilt(voltage(:, 1)-voltage(:, 2), 5, 501)))
title('Absolute Voltage Error [V]')


end

