....
Out1=(scale.read()); //1x údaj načten z HX
Out2=(scale.read_average(2)); //průměr nkrát čtení z HX
Out3=(scale.get_value(2));//průměr n1krát =( HX- tara)
Out4=(scale.get_units(2)); // průměr n2krát = (HX- tara)/g_factor;
delay(2000);
.....
