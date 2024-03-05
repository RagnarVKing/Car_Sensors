#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"

void read(sensor **vector, int n, FILE *f)
{
	*vector = malloc(n * sizeof(sensor));

	for (int i = 0; i < n; i++) {
		fread(&((*vector)[i].sensor_type), sizeof(int), 1, f);

		if ((*vector)[i].sensor_type == 1) {
			(*vector)[i].sensor_data = malloc(sizeof(power_management_unit));
			fread(&(((power_management_unit *)
				(*vector)[i].sensor_data)->voltage),
				sizeof(float), 1, f);
			fread(&(((power_management_unit *)
				(*vector)[i].sensor_data)->current),
				sizeof(float), 1, f);
			fread(&(((power_management_unit *)
				(*vector)[i].sensor_data)->power_consumption),
				sizeof(float), 1, f);
			fread(&(((power_management_unit *)
				(*vector)[i].sensor_data)->energy_regen),
				sizeof(int), 1, f);
			fread(&(((power_management_unit *)
				(*vector)[i].sensor_data)->energy_storage),
				sizeof(int), 1, f);
		}
		if ((*vector)[i].sensor_type == 0) {
			(*vector)[i].sensor_data = malloc(sizeof(tire_sensor));
			fread(&(((tire_sensor *)(*vector)[i].sensor_data)->pressure),
				sizeof(float), 1, f);
			fread(&(((tire_sensor *)(*vector)[i].sensor_data)->temperature),
				sizeof(float), 1, f);
			fread(&(((tire_sensor *)(*vector)[i].sensor_data)->wear_level),
				sizeof(int), 1, f);
			fread(&(((tire_sensor *)
				(*vector)[i].sensor_data)->performace_score),
				sizeof(int), 1, f);
		}

		fread(&(*vector)[i].nr_operations, sizeof(int), 1, f);

		(*vector)[i].operations_idxs =
		malloc((*vector)[i].nr_operations * sizeof(int));

		for (int j = 0; j < (*vector)[i].nr_operations; j++)
			fread(&(*vector)[i].operations_idxs[j], sizeof(int), 1, f);
	}
	fclose(f);
}

void print(sensor *vector, int n, int indx)
{
	if (indx < 0 || indx > n) {
		printf("Index not in range!\n");
		return;
	}
	int *v = malloc(n * sizeof(int));
	int j = 0;
	for (int i = 0; i < n; i++) {
		if (vector[i].sensor_type == 1) {
			v[j] = i;
			j++;
		}
	}
	for (int i = 0; i < n; i++) {
		if (vector[i].sensor_type == 0) {
			v[j] = i;
			j++;
		}
	}
	if (vector[v[indx]].sensor_type == 1) {
		printf("Power Management Unit\n");
		printf("Voltage: %.2f\n",
			((power_management_unit *)vector[v[indx]].sensor_data)->voltage);
		printf("Current: %.2f\n",
			((power_management_unit *)vector[v[indx]].sensor_data)->current);
		printf("Power Consumption: %.2f\n",
			((power_management_unit *)vector[v[indx]].sensor_data)
			->power_consumption);
		printf("Energy Regen: %d%%\n",
			((power_management_unit *)vector[v[indx]].sensor_data)
			->energy_regen);
		printf("Energy Storage: %d%%\n",
			((power_management_unit *)vector[v[indx]].sensor_data)
			->energy_storage);
	}
	if (vector[v[indx]].sensor_type == 0) {
		printf("Tire Sensor\n");
		printf("Pressure: %.2f\n",
			((tire_sensor *)vector[v[indx]].sensor_data)->pressure);
		printf("Temperature: %.2f\n",
			((tire_sensor *)vector[v[indx]].sensor_data)->temperature);
		printf("Wear Level: %d%%\n",
			((tire_sensor *)vector[v[indx]].sensor_data)->wear_level);
		if (((tire_sensor *)vector[v[indx]].sensor_data)
			->performace_score == 0)
			printf("Performance Score: Not Calculated\n");
		else
			printf("Performance Score: %d\n",
				((tire_sensor *)vector[v[indx]].sensor_data)
				->performace_score);
	}
	free(v);
}

void analyze(sensor *vector, int n, int indx)
{
	void (*operations[8])(void *);
	get_operations((void **)operations);
	if (indx < 0 || indx > n) {
		printf("Not in range\n");
		return;
	}
	int *v = malloc(n * sizeof(int));
	int j = 0;
	for (int i = 0; i < n; i++)
		if (vector[i].sensor_type == 1) {
			v[j] = i;
			j++;
		}
	for (int i = 0; i < n; i++)
		if (vector[i].sensor_type == 0) {
			v[j] = i;
			j++;
		}
	for (int j = 0; j < vector[v[indx]].nr_operations; j++)
		operations[vector[v[indx]].operations_idxs[j]]
		(vector[v[indx]].sensor_data);
	free(v);
}

void delete(sensor **vector, int *n, int poz)
{
	free((*vector)[poz].operations_idxs);
	free((*vector)[poz].sensor_data);
	int nr = *n;
	for (int i = poz; i < nr - 1; i++)
		(*vector)[i] = (*vector)[i + 1];
	*n = *n - 1;
	(*vector) = realloc((*vector), (nr - 1) * sizeof(sensor));
}

void clear(sensor **vector, int *n)
{
	for (int i = 0; i < *n; i++) {
		if ((*vector)[i].sensor_type == 1) {
			if (((power_management_unit *)
				(*vector)[i].sensor_data)->voltage < 10 ||
				((power_management_unit *)
				(*vector)[i].sensor_data)->voltage > 20) {
				delete (vector, n, i);
				i--;
				continue;
			}
			if (((power_management_unit *)
				(*vector)[i].sensor_data)->current < -100 ||
				((power_management_unit *)
				(*vector)[i].sensor_data)->current > 100) {
				delete (vector, n, i);
				i--;
				continue;
			}
			if (((power_management_unit *)
				(*vector)[i].sensor_data)->power_consumption < 0 ||
				((power_management_unit *)
				(*vector)[i].sensor_data)->power_consumption > 1000) {
				delete (vector, n, i);
				i--;
				continue;
			}
			if (((power_management_unit *)
				(*vector)[i].sensor_data)->energy_regen < 0 ||
				((power_management_unit *)
				(*vector)[i].sensor_data)->energy_regen > 100) {
				delete (vector, n, i);
				i--;
				continue;
			}
			if (((power_management_unit *)
				(*vector)[i].sensor_data)->energy_storage < 0 ||
				((power_management_unit *)
				(*vector)[i].sensor_data)->energy_storage > 100) {
				delete (vector, n, i);
				i--;
				continue;
			}
			continue;
		}
		if ((*vector)[i].sensor_type == 0) {
			if (((tire_sensor *)
				(*vector)[i].sensor_data)->pressure < 19 ||
				((tire_sensor *)(*vector)[i].sensor_data)->pressure > 28) {
				delete (vector, n, i);
				i--;
				continue;
			}
			if (((tire_sensor *)
				(*vector)[i].sensor_data)->temperature < 0 ||
				((tire_sensor *)
				(*vector)[i].sensor_data)->temperature > 120) {
				delete (vector, n, i);
				i--;
				continue;
			}
			if (((tire_sensor *)
				(*vector)[i].sensor_data)->wear_level < 0 ||
				((tire_sensor *)
				(*vector)[i].sensor_data)->wear_level > 100) {
				delete (vector, n, i);
				i--;
				continue;
			}
			continue;
		}
	}
}

void exitt(sensor **vector, int n)
{
	for (int i = 0; i < n; i++) {
		free((*vector)[i].operations_idxs);
		free((*vector)[i].sensor_data);
	}
	free((*vector));
}

int main(int argc, char const *argv[])
{
	sensor *vector; // declar vector

	FILE *f = fopen(argv[1], "rb"); // deschid fisier

	if (f == NULL) // daca fisier nu exista
		printf("Failed to load %s\n", argv[1]);

	int n; // numar sensori

	fread(&n, sizeof(int), 1, f);

	read(&vector, n, f);

	char command[10];
	int indx;
	while (1) {
		scanf("%s", command);
		if (strncmp(command, "print", 5) == 0) {
			scanf("%d", &indx);
			print(vector, n, indx);
			continue;
		} else if (strncmp(command, "analyze", 7) == 0) {
			scanf("%d", &indx);
			analyze(vector, n, indx);
			continue;
		} else if (strncmp(command, "clear", 4) == 0) {
			clear(&vector, &n);
			continue;
		} else if (strncmp(command, "exit", 4) == 0) {
			exitt(&vector, n);
			break;
		}
	}

	return 0;
}
