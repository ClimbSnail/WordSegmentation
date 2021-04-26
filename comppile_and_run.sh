rm -rf segmentation_app
rm -rf core.*
rm -rf translate_out
make cleanall && make && make clean
./segmentation_app  test_data_gb2312

