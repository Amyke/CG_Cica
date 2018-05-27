 # CG_Cica

## World of Warships

- [x] A vízfelület legyen egy kellően nagy méretű négyzet az XZ síkon, ami lassan hullámzik, különböző frekvenciájú szinuszoidok szerint X és Z irányban (tipp: a hullámzás könnyen megvalósítható a vertex shaderben!) (5%)
- [x] Bump mappinggal vagy normal mappinggal együtt lássuk el víztextúrával! (A szükséges elméletről ITT találsz diákat) (15%)

- [x] A pályát vegye körbe egy hegyvonulat; a hegyet alkotó vertexek X és Z pozíciója legyen fix, de az Y magasságuk legyen véletlen generált. Vigyázzunk, hogy a véletlen generálás során ne kapjunk absztrakt/lehetetlenül kinéző hegyeket! Azaz a hegyvonulatot alkotó vertexek közül az egymáshoz közel esők Y magasságértéke legyen hasonló! (5%)
- [x] A hegy színe az egyes vertexek/fregmensek esetén Y érték szerint változzon: azok a vertexek vagy fregmensek, akik vízszinten vannak, legyenek zöldek, és minél magasabban helyezkedik el egy vertex vagy fregmens, annál fehérebb! (5%)

- [x] A pálya két átellenes sarkán legyen egy-egy világítótorony (henger, tetején kúppal), mindkettőnek a csúcsában legyen egy-egy pontszerű fényforrás. Mindkét fényforrásnak legyen diffúz és spekuláris fénykomponense (ezek legyenek különbözőek!), illetve legyen ambiens fényünk is. (10%)
- [x] Legyen meg a két torony, ne csak a két fényforrás.

- [ ] Modellezzünk csatahajókat! A csatahajók legyenek textúrázottak; a modellezésnél nem kell túlzásba esni, de ránézésre fel lehessen ismerni, hogy csatahajóról van szó! (5%)
- [x] Minden csatahajónak legyen lövegtornya (sokszög alapú hasábból és hengerből), a lövegtornyok külön tudnak forogni: egyrészt körbe tudjuk forgatni 360 fokkal, másrészt fel-le tudjuk állítani bizonyos dőlésszögben (vízszinteshez képest lefele 20 fokkal, felfele 60 fokkal). Több hajót is helyezzünk el a színtéren (legalább ötöt), ezek közül egyet mi fogunk irányítani, a többit a "mesterséges intelligencia". (5%)

- [ ] A hajók haladni tudnak a vízen. A hajók az alábbi mozgára képesek: mozgás előre, mozgás hátra, fordulás, valamint a lövegtorony kezelése. A mozgás minden esetben legyen sima, azaz a fordulás is legyen animált (ne "ugorjon" a hajó). A hajók közül egyet mi irányíthatunk, a többi esetén az irányításba nincs beleszólásunk. Minden hajó követi a víz hullámzását! (Sose merül el, de nem is lebeg a víz felett.) (10%)
- [x] A hajók gyorsulva illetve lassulva haladjanak (egy bizonyos maximális sebességig), ha fordulni kezdenek, veszítenek a sebességükből. (5%)

- [ ] A hajók lövegtornyával lehessen lőni. Ekkor egy szürke henger (a lövedék) hagyja el a lövegtornyot az adott irányba. A lövedékre hat a gravitáció, azaz a lövedék egy parabola-pályát ír le. (5%)
- [ ] Ha a lövedék eltalált egy hajót, akkor a hajó megsérült: minden hajó 10 pontnyi "életerővel" rendelkezik, a találat 2-5 pontot vesz ebből le (véletlen generált érték). Ha a lövedék nem találta el hajót, akkor eltűnik ha a magassága a vízszint alá esik. Hegybe történő becsapódást nem kell vizsgálni. (5%)
- [ ] Minden hajó fölött legyen egy "életerőt" jelző csík (téglalap), ez mindig úgy álljon, hogy a kamera irányába néz (azaz bárhogy forognak a hajók és bárhol van a kamera, mindig teljes szélességben látjuk a csíkot, de a csík követi a hajó haladását) (10%)
- [ ] Ha egy hajó megsemmisül, akkor animálva elsüllyed: orral lefelé fordul, majd elindul a mélybe, és eltűnik ha már nem látjuk. (5%)

- [x] A kameránk végig követi a hajónkat (felette lebeg), de szabadon tudjuk körbeforgatni. Szóköz billentyű hatására a kamera "elengedi" a hajónkat, ekkor a pályát szabadon bejárhatjuk, de a szóköz újbóli megnyomására visszaugrunk a hajónkhoz. (5%)

- [x] Az ellenséges hajók maguktól mozogjanak és támadjanak. A mozgásuk legyen véletlen, de csak 5 mp-enként változzon (például elfordul valamekkora szöggel, majd halad 5 mp-ig, majd megint elfordul, megint halad, és így tovább). (5%)
- [x] Nem úsznak ki a pálya széléről! (5%) 
- [ ]  Ha közel vannak hozzánk, akkor felénk fordítják a lövegtornyukat, és tüzelnek, ezt megpróbálják a lehető legpontosabban véghez vinni: ehhez számítsuk ki a lövegtorony állását! A víz hullámzását ne vegyük figyelembe, így persze időnként mellélőnek. (15%)
- [ ] Az ellenséges hajók csak bizonyos időközönként tüzelnek újra. (Ha így is gyorsan kilőnének minket, állítsuk nyugodtan magasabbra a mi hajónk kezdeti életerejét.)

- [ ] A mi hajónk orrán legyen egy reflektor (spot lámpa), ami mindig előrefele áll. A reflektornak legyen diffúz komponense. Ügyeljünk rá, hogy a reflektor nem csak egy pontszerű fényforrás, hanem csak adott szögtartományban világít, egy bizonyos irányba! Ezen felül a spot lámpánk fényerőssége a távolság függvényében fokozatosan csökkenjen. (10%)
