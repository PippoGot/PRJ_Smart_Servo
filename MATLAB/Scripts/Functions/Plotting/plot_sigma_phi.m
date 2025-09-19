function plot_sigma_phi(dataset)
%PLOT_SIGMA_PHI Summary of this function goes here
%   Plots a phase portrait in the sigma-phi-omega space.
%   sigma   =   -Ki(theta-ref)              Tracking Error
%   phi     =   -Kp(theta-ref) - Ki*xc      Control Effort/Voltage
%   omega   =   w                           Velocity

integrator = tf('s')^-1;
xc = lsim(integrator, dataset.position - dataset.reference, dataset.time);

sigma = 50*(dataset.position - dataset.reference);
phi = 10*(dataset.position - dataset.reference) + 50*xc;
omega = dataset.velocity;


plot3(sigma, phi, omega);
title('Stick-Slip Coordinates')
xlabel('sigma')
ylabel('phi')
zlabel('omega')
grid on

end

