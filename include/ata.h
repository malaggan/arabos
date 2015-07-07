using sector_t = size_t;

void write_sector(uint16_t const data[256], sector_t sect_num);
void read_sector (uint16_t data[256], sector_t sect_num);
