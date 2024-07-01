idf_component_register(SRCS "airsafe.c"
                    INCLUDE_DIRS "."
					REQUIRES driver
					PRIV_REQUIRES HD44780
)
