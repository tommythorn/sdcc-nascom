#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef char C;typedef int I;
typedef struct a *A;
struct a {
  I t,r,d[3];
  union member {
    I pi[1];
    A pa[1];
  } u;
};
#define P printf
#define R return
#define V1(f) A f(A w)
#define V2(f) A f(A a,A w)
#define DO(n,x) {I i=0,_n=(n);for(;i<_n;++i){x;}}
A ma(I n){R(A)malloc(sizeof(struct a)+n*sizeof(A));}
void mv(I *d,I *s,I n){DO(n,d[i]=s[i]);}
I tr(I r,I *d){I z=1;DO(r,z=z*d[i]);R z;}
A ga(I t,I r,I *d){A z=ma(tr(r,d));z->t=t,z->r=r,mv(z->d,d,r); R z;}
V1(iota){I n=*w->u.pi;A z=ga(0,1,&n);DO(n,z->u.pi[i]=i);R z;}
V2(plus){I r=w->r,*d=w->d,n=tr(r,d);A z=ga(0,r,d);
        DO(n,z->u.pi[i]=a->u.pi[i]+w->u.pi[i]);R z;}
V2(from){I r=w->r-1,*d=w->d+1,n=tr(r,d);
  A z=ga(w->t,r,d);mv(z->u.pi,w->u.pi+(n**a->u.pi),n);R z;}
V1(box){A z=ga(1,0,0);*z->u.pa=w;R z;}
V2(cat){I an=tr(a->r,a->d),wn=tr(w->r,w->d),n=an+wn;
  A z=ga(w->t,1,&n);mv(z->u.pi,a->u.pi,an);mv(z->u.pi+an,w->u.pi,wn);R z;}
V2(find){(void) a;(void) w;R 0;}
V2(rsh){I r=a->r?*a->d:1,n=tr(r,a->u.pi),wn=tr(w->r,w->d);
  A z=ga(w->t,r,a->u.pi);mv(z->u.pi,w->u.pi,wn=n>wn?wn:n);
  if(n-=wn)mv(z->u.pi+wn,z->u.pi,n);R z;}
V1(sha){A z=ga(0,1,&w->r);mv(z->u.pi,w->d,w->r);R z;}
V1(id){R w;}V1(size){A z=ga(0,0,0);*z->u.pi=w->r?*w->d:1;R z;}
void pi(I i){P("%d ",i);}void nl(void){putchar('\n');}
void pr(A w){I r=w->r,*d=w->d,n=tr(r,d);DO(r,pi(d[i]));nl();
  if(w->t)DO(n,P("< ");pr(w->u.pa[i]))else DO(n,pi(w->u.pi[i]));nl();}

static const C vt[]="+{~<#,";
static const A(*vd[])(A,A)={0,plus,from,find,0,rsh,cat};
static const A (*vm[])(A)={0,id,size,iota,box,sha,0};
A st[26]; I qp(I a){R a>='a'&&a<='z';}
I qv(I a){R a<'a';}
A ex(A *e){A a=*e;
  if (qp((I)a)) {
      if (e[1]==(A)(I)'=') R st[(I)a-'a'] = ex(e+2);
      a = st[(I)a-'a'];
  }
  R qv((I)a)?(*vm[(I)a])(ex(e+1)):e[1]?(*vd[(I)e[1]])(a,ex(e+2)):a;}
A noun(I c){A z;if(c<'0'||c>'9')R 0;z=ga(0,0,0);*z->u.pi=c-'0';R z;}
A verb(I c){I i=0;for(;vt[i];)if(vt[i++]==c)R (A)i;R 0;}
A *wd(C *s){I n=strlen(s); A a,*e=(A*)ma(n+1); C c;
    DO(n,e[i]=(a=noun(c=s[i]))?a:(a=verb(c))?a:(A)(I)c);e[n]=0; R e;}

// Emscripten's gets() appears to not strip the newline
int gets_(char *s)
{
    char *s_start = s;

    printf("   ");

    for (;;) {
        int ch = getchar();

        if (ch >= ' ') {
            *s++ = ch;
            putchar(ch);
        } else if (ch == 21) { // ^U
            for (; s != s_start; --s)
                putchar(8);
        } else if (ch == 8) { // ^H
            if (s != s_start) {
                --s;
                putchar(ch);
            }
        } else if (ch == '\n') {
            putchar(ch);
            *s = 0;
            return s != s_start;
        } else {
            printf("<%02x?>", ch);
        }
    }
}

extern char _xinit__vd[];

int main()
{
    C s[99];

    printf("Nascom J v0.0.2\n");

    while (gets_(s))
      pr(ex(wd(s)));

    R 0;
}
