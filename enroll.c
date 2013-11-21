#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <libfprint/fprint.h>

struct fp_dscv_dev *discover_device(struct fp_dscv_dev **discovered_devs)
{
	struct fp_dscv_dev *ddev = discovered_devs[0];
	struct fp_driver *drv;
	if (!ddev)
		return NULL;
	
	drv = fp_dscv_dev_get_driver(ddev);
	printf("Found %s\n", fp_driver_get_full_name(drv));
	return ddev;
}

struct fp_print_data *enroll(struct fp_dev *dev) {
	struct fp_print_data *enrolled_print = NULL;
	int r;

	printf("You will need to scan your finger %d times to "
		"complete the process.\n", fp_dev_get_nr_enroll_stages(dev));

	do {
		struct fp_img *img = NULL;
	
		sleep(1);
		printf("\nScan your finger now.\n");

		r = fp_enroll_finger_img(dev, &enrolled_print, &img);
		if (img) {
			fp_img_save_to_file(img, "enrolled.pgm");
			printf("Wrote scanned image to enrolled.pgm\n");
			fp_img_free(img);
		}
		if (r < 0) {
			printf("Enroll failed \n");
			return NULL;
		}

	} while (r != FP_ENROLL_COMPLETE);

	if (!enrolled_print) {
		printf("Enroll complete but no print?\n");
		return NULL;
	}

	printf("Enrollment completed!\n\n");
	return enrolled_print;
}

int main(void)
{
	int r = 1;
	struct fp_dscv_dev *ddev;
	struct fp_dscv_dev **discovered_devs;
	struct fp_dev *dev;
	struct fp_print_data *data;

	r = fp_init();
	if (r < 0) {
		printf("Failed to initialize libfprint\n");
		exit(1);
	}
	fp_set_debug(3);

	discovered_devs = fp_discover_devs();
	if (!discovered_devs) {
		printf("Could not discover devices\n");
		goto out;
	}

	ddev = discover_device(discovered_devs);
	if (!ddev) {
		printf("No devices detected.\n");
		goto out;
	}

	dev = fp_dev_open(ddev);
	fp_dscv_devs_free(discovered_devs);
	if (!dev) {
		printf("Could not open device.\n");
		goto out;
	}

	printf("Opened device. enroll your finger.\n\n");
	data = enroll(dev);
	if (!data)
		goto out_close;

	r = fp_print_data_save(data, RIGHT_INDEX);
	if (r < 0)
		printf("Data save failed, code %d\n");

	fp_print_data_free(data);
out_close:
	fp_dev_close(dev);
out:
	fp_exit();
	return r;
}


