idf_component_register(SRCS "GDEP133C02.c"
                            "usbcdc.c"                            
                            "main.c"
                            "comm.c"
                         INCLUDE_DIRS "."
                         EMBED_TXTFILES howsmyssl_com_root_cert.pem
                         postman_root_cert.pem)

