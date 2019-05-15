figure("Name","Filter Comparison");
hold on 
plot(filterdata(:,1), filterdata(:,2));
plot(filterdata(:,1), filterdata(:,3));
plot(filterdata(:,1), filterdata(:,4));
plot(filterdata(:,1), filterdata(:,5));
legend("Raw", "MAF", "IIR", "FIR");
title("MAF, IIR, FIR Filter Comparison");
xlabel("Sample");
ylabel("Raw Acceleration Value");
%%
figure("Name","Filter Comparison");
hold on 
shaking = shakingfiltercomparsion;
plot(shaking(:,1), shaking(:,2));
plot(shaking(:,1), shaking(:,3));
plot(shaking(:,1), shaking(:,4));
plot(shaking(:,1), shaking(:,5));
legend("Raw", "MAF", "IIR", "FIR");
title("MAF, IIR, FIR Filter Comparison");
xlabel("Sample");
ylabel("Raw Acceleration Value");