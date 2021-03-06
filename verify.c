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

int verify(struct fp_dev *dev, struct fp_print_data *data)
{
	int r;

	do {
		struct fp_img *img = NULL;

		sleep(1);
		printf("\nScan your finger now.\n");
		r = fp_verify_finger_img(dev, data, &img);
		if (img) {
			fp_img_save_to_file(img, "verify.pgm");
			printf("Wrote scanned image to verify.pgm\n");
			fp_img_free(img);
		}
		if (r < 0) {
			printf("verification failed\n");
			return r;
		}
	} while (1);
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
		printf("Failed to initialize fprint\n");
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

	printf("Opened device.  "
		"enrolling...\n");

	r = fp_print_data_load(dev, RIGHT_INDEX, &data);
	if (r != 0) {
		printf("Failed to load fingerprint,\n");
		goto out_close;
	}

	printf("Print loaded. Try to verify!\n");
	do {
		char buffer[20];

		verify(dev, data);
		printf("Verify again? [Y/n]? ");
		fgets(buffer, sizeof(buffer), stdin);
		if (buffer[0] != '\n' && buffer[0] != 'y' && buffer[0] != 'Y')
			break;
	} while (1);

	fp_print_data_free(data);
out_close:
	fp_dev_close(dev);
out:
	fp_exit();
	return r;
}


