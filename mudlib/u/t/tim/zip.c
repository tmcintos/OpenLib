/*  -*- LPC -*-  */
#pragma optimize

class heapnode {
  class heapnode left;
  class heapnode right;
  int freq;
  char ch;
}

int
pow2(int n)
{
  switch(n) {
  case 0:
    n = 1;
    break;
  case 1:
    n = 2;
    break;
  case 2:
    n = 4;
    break;
  case 3:
    n = 8;
    break;
  case 4:
    n = 16;
    break;
  case 5:
    n = 32;
    break;
  case 6:
    n = 64;
    break;
  case 7:
    n = 128;
    break;
  }
  return n;
}

int
setb(int byte, int n)
{
  return byte | pow2(n);
}

int
clearb(int byte, int n)
{
  return byte & (pow2(n) ^ 255);
}

int
testb(int byte, int n)
{
  return (byte & pow2(n) ? 1 : 0);
}


mapping
huffman_encode(class heapnode h, int* encoding)
{
  if( h->ch != -1 ) {
    return ([ h->ch : encoding ]);
  } else {
    return ((h->left ? huffman_encode(h->left, encoding + ({ 0 })) : ([])) +
            (h->right ? huffman_encode(h->right, encoding + ({ 1 })) : ([])));
  }
}

int
compare_heapnodes(class heapnode h1, class heapnode h2)
{
  if(h1->freq > h2->freq)
    return 1;
  else if(h1->freq == h2->freq)
    return 0;
  else
    return -1;
}

// creates a heap from a frequency mapping

class heapnode
create_heap(mapping freqmap)
{
  class heapnode* L = ({});

  // create heap from frequency mapping
  foreach(char ch in keys(freqmap)) {
    class heapnode newnode = new(class heapnode);

    newnode->ch = ch;
    newnode->freq = freqmap[ch];
    L += ({ newnode });
  }

  while( sizeof(L) > 1 ) {
    class heapnode newnode = new(class heapnode);

    L = sort_array(L, (: compare_heapnodes :));
    newnode->left = L[0];
    newnode->right = L[1];
    newnode->freq = L[0]->freq + L[1]->freq;
    newnode->ch = -1;
    L -= ({ L[0], L[1] });
    L += ({ newnode });
  }
  // assert that L has only 1 element, root of heap
  return L[0];
}

// creates a character encoding mapping from a group of strings
mapping
create_freqmap(string str)
{
  mapping result = ([]);

  // create character frequency mapping
  foreach(char ch in str)
    result[ch]++;

  return result;
}

// given character encoding  mapping compresses string and returns a buffer
buffer
compress(mapping F, string str)
{
  buffer buf = allocate_buffer(1);
  buffer size = allocate_buffer(4);
  int i; // current bit in buffer

  foreach(char ch in str) {
    foreach(int bit in F[ch]) {
      int currbyte = i / 8;
      int relbit = 7 - (i % 8);
      if( sizeof(buf) <= currbyte ) buf += allocate_buffer(1);
      if( bit )
	buf[currbyte] = setb(buf[currbyte], relbit);
      else
	buf[currbyte] = clearb(buf[currbyte], relbit);
      i++;
    }
  }

  write_buffer(size, 0, i);
  return size + buf;
}

string
uncompress(mapping F, buffer buf)
{
  string ret = "";
  int numbits = to_int(buf[0..3]);

  buf = buf[4..];
  for(int i = 0; i < numbits; i++) {
    foreach(char ch, int* encoding in F) {
      int found = 1;
      for(int j = 0; j < sizeof(encoding); j++) {
	int bit = i + j;
	if( encoding[j] != testb(buf[bit / 8], 7 - (bit % 8)) ) {
	  found = 0;
	  break;
	}
      }
      if( found ) {
	i += sizeof(encoding) - 1;
	ret += sprintf("%c", ch);
	break;
      }
    }
  }
  return ret;
}

mixed
test()
{
  string s = read_file("/www/index.html~");
  mapping F;

  F = create_freqmap(s);
  F = huffman_encode(create_heap(F), ({}));
  call_out("finish", 1, F, s);
}

void
finish(mapping F, string s)
{
  buffer b;
  b = compress(F, s);
//  for(int j = 0; j < sizeof(b); j++) {
//    for(int i = 0; i < 8; i++) printf("%i", testb(b[j], 7-i));
//    write("\n");
//  }
//  for(int i = 0; i < sizeof(b); i++)
//    printf("%i %i\n", i, b[i]);
  tell_object(find_player("tim"),sprintf("sizeof s=%i, sizeof b=%i\n", sizeof(s), sizeof(b)));
//  return uncompress(b);
//  return huffman_encode(create_heap(F), ({}));
//  return uncompress(F, compress(huffman_encode(create_heap(F), ({})), s));
//  return uncompress(F, compress(F, s));
//  return F;
}
