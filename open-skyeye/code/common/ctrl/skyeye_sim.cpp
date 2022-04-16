
void init_skyeye_sim()
{
    static skyeye_class_t class_data = {
        .class_name = "skyeye_sim",
        .class_desc = "global simulator data for SkyEye",
        .new_instance = new_am35x_hecc,
        .free_instance = free_am35x_hecc,
        .get_attr = NULL,
        .set_attr = NULL
    };

    SKY_register_class(class_data.class_name, &class_data);
}
