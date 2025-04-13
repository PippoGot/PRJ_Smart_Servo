close all

%% Simulation Loop

sim_time = 20;      % Total simulation time             [s]
sim_blocks = 6;     % Number of models in parallel      [#]
sim_step = 1e-3;    % Time step of simulation           [s]
sim_samples = sim_time / sim_step + 1;      % Total number of samples   [#]

for i = 1:1:25
    % Generate initial conditions
    R = [rand(4, sim_blocks)];
    T = diag([0, 24, 2, 2]);
    IC = T*R - 0.5*T*ones(size(R));
    icl = IC(1:3, :);
    icn = IC;

    % Run simulation for 20 seconds
    sim("Phase_Space_Test.slx");

    % Gather and restructures data
    time = ans.simout.time;
    q_lin = zeros(3, sim_samples, sim_blocks);
    q_nlin = zeros(3, sim_samples, sim_blocks);

    for j = 1:1:sim_blocks
        rows_lin = 3*[j-1, j] + [1, 0];
        rows_nlin = rows_lin + 3*sim_blocks;
 
        q_lin(:, :, j) = reshape(ans.simout.signals.values(rows_lin(1):rows_lin(end), :, :), 3, []);
        q_nlin(:, :, j) = reshape(ans.simout.signals.values(rows_nlin(1):rows_nlin(end), :, :), 3, []);
    end
    
    % Plot data
    figure(1);
    hold on
    grid on
    xlim([-0.2, 0.2]);
    ylim([-25, 25]);
    zlim([-2.5, 2.5]);
    xlabel("Position [rad]");
    ylabel("Velocity [rad/s]");
    zlabel("Current [A]");
    
    for j = 1:1:sim_blocks
%         plot3(q_lin(1, :, j), q_lin(2, :, j), q_lin(3, :, j), "b-");
%         plot3(q_nlin(1, :, j), q_nlin(2, :, j), q_nlin(3, :, j), "r-");

        plot3(q_lin(1, 2:end, j), q_lin(2, 2:end, j), q_lin(3, 2:end, j), "b-");
        plot3(q_nlin(1, 2:end, j), q_nlin(2, 2:end, j), q_nlin(3, 2:end, j), "r-");
    end
end

hold off


% Clear unused variables
clear R
clear T
clear IC
clear icl
clear icn