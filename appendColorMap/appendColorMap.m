clear
map = load('pascal_colormap.mat');
in = dir('in');
out = 'out';
for i = 3:length(in)
    fn = string(in(i).folder)+'/'+string(in(i).name);
    img_cm = imread(fn{1,1});
    out_fn = string(out) + '/'+string(in(i).name);
    imwrite(img_cm, map.colormap, out_fn{1,1});
end
