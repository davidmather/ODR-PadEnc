/*
    Copyright (C) 2014 CSP Innovazione nelle ICT s.c.a r.l. (http://rd.csp.it/)

    Copyright (C) 2014, 2015 Matthias P. Braendli (http://opendigitalradio.org)

    Copyright (C) 2015, 2016, 2017 Stefan Pöschel (http://opendigitalradio.org)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*!
    \file pad_common.h
    \brief Common parts related to PAD

    \author Sergio Sagliocco <sergio.sagliocco@csp.it>
    \author Matthias P. Braendli <matthias@mpb.li>
    \author Stefan Pöschel <odr@basicmaster.de>
*/

#include <cstdio>
#include <vector>
#include <deque>
#include <cstring>
#include <string>
#include <stdint.h>
#include <unistd.h>

#include "crc.h"



typedef std::vector<uint8_t> uint8_vector_t;
typedef uint8_vector_t pad_t;



// --- DATA_GROUP -----------------------------------------------------------------
struct DATA_GROUP {
    uint8_vector_t data;
    int apptype_start;
    int apptype_cont;
    size_t written;

    DATA_GROUP(size_t len, int apptype_start, int apptype_cont);
    void AppendCRC();
    size_t Available();
    int Write(uint8_t *write_data, size_t len, int *cont_apptype);
};


// --- PADPacketizer -----------------------------------------------------------------
class PADPacketizer {
private:
    const size_t xpad_size_max;
    const bool short_xpad;
    const size_t max_cis;

    size_t xpad_size;
    uint8_t subfields[4*48];
    size_t subfields_size;

    // PAD w/  CI list
    int ci_type[4];
    size_t ci_len_index[4];
    size_t used_cis;

    // PAD w/o CI list
    int last_ci_type;
    size_t last_ci_size;

    size_t AddCINeededBytes();
    void AddCI(int apptype, int len_index);

    int OptimalSubFieldSizeIndex(size_t available_bytes);
    int WriteDGToSubField(DATA_GROUP* dg, size_t len);

    bool AppendDG(DATA_GROUP* dg);
    void AppendDGWithCI(DATA_GROUP* dg);
    void AppendDGWithoutCI(DATA_GROUP* dg);

    void ResetPAD();
    pad_t* FlushPAD();
public:
    static const size_t SUBFIELD_LENS[];
    static const size_t FPAD_LEN;
    static const size_t SHORT_PAD;
    static const size_t VARSIZE_PAD_MIN;
    static const size_t VARSIZE_PAD_MAX;
    static const std::string ALLOWED_PADLEN;

    std::deque<DATA_GROUP*> queue;

    PADPacketizer(size_t pad_size);
    ~PADPacketizer();

    pad_t* GetPAD();

    // will be removed, when pull (instead of push) approach is implemented!
    void WriteAllPADs(int output_fd);
};