/*
This file is part of Diesel
(c) 2002 by Mathias Heyer
email: sonode@gmx.de

Diesel is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Diesel is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
/** quicksort needs operators < and > to be defined for elementtype*/

/*template< const int max_stackdepth,class _T>
void quicksort(_T *array, int num_elements, const int threshold=50)
{

int stack[max_stackdepth];
int stackdepth=0;

_T pivot;
_T temp;

int Li=0;
int Ri=num_elements-1;
int	li,ri,n,j;

_T *tarray;

    li=0;ri=Ri;

LOOP:
    {
        if ((n=(Ri-Li))>threshold)
        {
            j=(Li+Ri)>>1;

            // get median-of-three (gains no performance)
            if (array[Li]>array[j])
            {
                if (array[Li]<array[Ri]) j=Li;
            }
            else if (array[Ri]<array[j]) j=Ri;

            pivot=array[j];
            while (li<ri)
            {
                while(array[li]<pivot) ++li;
                while(array[ri]>pivot) --ri;
                if (li<=ri)
                {
                    temp=array[li];array[li]=array[ri];array[ri]=temp;
                    ++li;--ri;
                }
            }
            if (Li<ri)
            {
                stack[stackdepth++]=li;
                li=Li;
                stack[stackdepth++]=Ri;
                Ri=ri;
                goto LOOP;
            }
            if (li<Ri)
            {
                Li=li;
                ri=Ri;
                goto LOOP;
            }
        }
        else
        {
            tarray=&array[Li];
            for (int i=1;i<=n;++i) //<= weil Li-Ri indizes sind (9 minus 0 sind 10 elemente)
            {
                    pivot=tarray[i];
                    j=i;
                    while((tarray[j-1]>pivot)&&j)
                    {
                        tarray[j]=tarray[j-1];
                        --j;
                    }
                    if (i!=j) tarray[j]=pivot;
            }
        }
        stackdepth-=2;
        if (stackdepth>=0)
        {
            Li=li=stack[stackdepth];
            Ri=ri=stack[stackdepth+1];
            goto LOOP;
        }
    };
}

*/

template <const int max_stackdepth, class _T>
void quicksort(_T* array, const int num_elements, const int threshold = 50)
{

    int stack[max_stackdepth];
    int stackdepth = 0;

    _T pivot;
    _T temp;

    int Li = 0;
    int li = 0;
    int Ri = num_elements - 1;
    int ri = Ri;
    int j;

LOOP:

    if ((Ri - Li) > threshold) {
        j = (Li + Ri) >> 1;

        // get median-of-three
        if (array[j] < array[Li]) {
            if (array[Li] < array[Ri])
                j = Li;
        } else if (array[Ri] < array[j])
            j = Ri;

        pivot = array[j];
        while (li < ri) {
            while (array[li] < pivot)
                ++li;
            while (pivot < array[ri])
                --ri;
            if (li <= ri) {
                temp        = array[li];
                array[li++] = array[ri];
                array[ri--] = temp;
            }
        }
        //	if (Ri-li > ri-Li)
        //	{
        if (Li < ri) {
            stack[stackdepth++] = li;
            li                  = Li;
            stack[stackdepth++] = Ri;
            Ri                  = ri;
            goto LOOP;
        }
        if (li < Ri) {
            Li = li;
            ri = Ri;
            goto LOOP;
        }
        /*	}
            else
            {
                if (li<Ri)
                {
                    stack[stackdepth++]=Li;
                    Li=li;
                    stack[stackdepth++]=ri;
                    ri=Ri;
                    goto LOOP;
                }
                if (Li<ri)
                {
                    li=Li;
                    Ri=ri;
                    goto LOOP;
                }
            }*/
    }
    if (stackdepth) {
        Ri = ri = stack[--stackdepth];  // get backwards from stack
        Li = li = stack[--stackdepth];
        goto LOOP;
    }

    // finally go through all elements with insertion sort
    for (int i = 1; i < num_elements; ++i) {
        pivot = array[i];
        j     = i;
        while (j && pivot < array[j - 1]) {
            array[j] = array[j - 1];
            --j;
        }
        if (i != j)
            array[j] = pivot;
    }
}