function plot_track_error(dataset_pid, dataset_swpid)
%PLOT_TRACK_ERROR Summary of this function goes here
%   Plot tracking error for velocity or position references.

time = dataset_pid.time;
ref = dataset_pid.reference;
data_pid = dataset_pid.position;
data_swpid = dataset_swpid.position;

subplot(2, 1, 1)
plot(time, ref, time, data_pid, time, data_swpid, 'LineWidth', 2)
grid on
title('PID and SWPID Comparison')
xlabel('Time [s]')
ylabel('Position [rad/s]')
legend('Reference', 'PID', 'SWPID')

subplot(2, 1, 2)
LSB = 2*pi/4096;
plot(time, ref-data_pid, time, ref-data_swpid, time, LSB*[ones(size(data_pid)), -ones(size(data_pid))], 'k--', time, 3*LSB*[ones(size(data_pid)), -ones(size(data_pid))], 'k:', 'LineWidth', 2)
grid on
% ylim([-5*LSB, 5*LSB])
xlabel('Time [s]')
ylabel('Position Error [rad/s]')
legend('PID', 'SWPID', 'Sensor LSB', '3*Sensor LSB')

end

