function plot_run(dataset, tp)
%PLOT_TRACK_ERROR Summary of this function goes here
%   Plot tracking error for velocity or position references.

time = dataset.time;
if strcmp(tp, 'position')
    position = [dataset.reference, dataset.position];
    velocity = dataset.velocity;
else
    position = dataset.position;
    velocity = [dataset.reference, dataset.velocity];
end
current = dataset.current;
voltage = dataset.voltage;


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

end

