%% Script to calculate the model jacobian via symbolic expressions

% Symbolic System Variables
syms th w i z v;
syms Ra La Kphi Wo E;
syms Be Je Kg Tc Tsb Wsb s0 s1;


% Symbolic Model Dynamic Equations
TAU = abs(w)/(Tc + (Tsb - Tc)*exp(-(w/(Kg*Wsb))^2));
PHI = Wo/sqrt(w^2 + E^2);

d_th = w;
d_w = 1/Je*(Kphi*Kg*i - (Be + s1)*w - s0*Kg*z + s0*s1*z*TAU);
d_i = 1/La*(v - Kphi*w/Kg - Ra*i - PHI*w/Kg);
d_z = 1/Kg*(w - s0*z*TAU);


% Symbolic Jacobian
J = jacobian([d_th, d_w, d_i, d_z], [th, w, i, z]);