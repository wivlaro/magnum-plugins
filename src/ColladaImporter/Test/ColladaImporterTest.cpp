/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "ColladaImporterTest.h"

#include <QtTest/QTest>

#include "Utility/Directory.h"
#include "Trade/PhongMaterialData.h"
#include "Trade/MeshData.h"
#include "../ColladaImporter.h"

#include "ColladaImporterTestConfigure.h"

using namespace std;
using namespace Corrade::Utility;

QTEST_APPLESS_MAIN(Magnum::Trade::ColladaImporter::Test::ColladaImporterTest)

namespace Magnum { namespace Trade { namespace ColladaImporter { namespace Test {

void ColladaImporterTest::openWrongNamespace() {
    ColladaImporter importer;
    stringstream debug;
    Error::setOutput(&debug);
    QVERIFY(!importer.open(Directory::join(COLLADAIMPORTER_TEST_DIR, "openWrongNamespace.dae")));
    QVERIFY(debug.str() == "ColladaImporter: unsupported namespace \"http://www.collada.org/2006/11/COLLADASchema\"\n");
}

void ColladaImporterTest::openWrongVersion() {
    ColladaImporter importer;
    stringstream debug;
    Error::setOutput(&debug);
    QVERIFY(!importer.open(Directory::join(COLLADAIMPORTER_TEST_DIR, "openWrongVersion.dae")));
    QVERIFY(debug.str() == "ColladaImporter: unsupported version \"1.4.0\"\n");
}

void ColladaImporterTest::parseSource() {
    ColladaImporter importer;
    QVERIFY(importer.open(Directory::join(COLLADAIMPORTER_TEST_DIR, "parseSource.dae")));

    stringstream debug;
    Error::setOutput(&debug);
    QVERIFY(importer.parseSource<Vector3>("WrongTotalCount") == vector<Vector3>());
    QVERIFY(debug.str() == "ColladaImporter: wrong total count in source \"WrongTotalCount\"\n");

    QEXPECT_FAIL(0, "Swapped coordinates in source are not implemented", Continue);
    QVERIFY(importer.parseSource<Vector3>("SwappedCoords") == vector<Vector3>{Vector3(0, 1, 2)});

    QVERIFY((importer.parseSource<Vector4>("MoreElements") == vector<Vector4>{
        Vector4(0, 1, 2),
        Vector4(3, 4, 5)
    }));
}

void ColladaImporterTest::scene() {
    ColladaImporter importer;
    QVERIFY(importer.open(Directory::join(COLLADAIMPORTER_TEST_DIR, "scene.dae")));

    QVERIFY(importer.sceneCount() == 2);
    QVERIFY(importer.objectCount() == 4);
}

void ColladaImporterTest::mesh() {
    ColladaImporter importer;
    QVERIFY(importer.open(Directory::join(COLLADAIMPORTER_TEST_DIR, "mesh.dae")));

    QVERIFY(importer.meshCount() == 4);

    stringstream debug;
    Error::setOutput(&debug);
    QVERIFY(!importer.mesh(0));
    QVERIFY(debug.str() == "ColladaImporter: 4 vertices per face not supported\n");

    /* Vertex only mesh */
    QVERIFY(!!importer.mesh(1));
    MeshData* mesh = importer.mesh(1);
    QVERIFY(mesh->primitive() == Mesh::Primitive::Triangles);
    QVERIFY((*mesh->indices() == vector<unsigned int>{
        0, 1, 2, 0, 2, 3, 4, 0, 3, 4, 3, 5
    }));
    QVERIFY(mesh->vertexArrayCount() == 1);
    QVERIFY((*mesh->vertices(0) == vector<Vector4>{
        {1, -1, 1},
        {1, -1, -1},
        {1, 1, -1},
        {1, 1, 1},
        {-1, -1, 1},
        {-1, 1, 1}
    }));
    QVERIFY(mesh->normalArrayCount() == 0);
    QVERIFY(mesh->textureCoords2DArrayCount() == 0);

    /* Vertex and normal mesh */
    QVERIFY(!!importer.mesh(2));
    mesh = importer.mesh(2);
    QVERIFY(mesh->primitive() == Mesh::Primitive::Triangles);
    QVERIFY((*mesh->indices() == vector<unsigned int>{
        0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7
    }));
    QVERIFY(mesh->vertexArrayCount() == 1);
    QVERIFY((*mesh->vertices(0) == vector<Vector4>{
        {1, -1, 1},
        {1, -1, -1},
        {1, 1, -1},
        {1, 1, 1},
        {-1, -1, 1},
        {1, -1, 1},
        {1, 1, 1},
        {-1, 1, 1}
    }));
    QVERIFY(mesh->normalArrayCount() == 1);
    QVERIFY((*mesh->normals(0) == vector<Vector3>{
        {1, 0, 0},
        {1, 0, 0},
        {1, 0, 0},
        {1, 0, 0},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1}
    }));
    QVERIFY(mesh->textureCoords2DArrayCount() == 0);

    /* Vertex, normal and texture mesh */
    QVERIFY(!!importer.mesh(3));
    mesh = importer.mesh(3);
    QVERIFY(mesh->primitive() == Mesh::Primitive::Triangles);
    QVERIFY((*mesh->indices() == vector<unsigned int>{
        0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7
    }));
    QVERIFY(mesh->vertexArrayCount() == 1);
    QVERIFY((*mesh->vertices(0) == vector<Vector4>{
        {1, -1, 1},
        {1, -1, -1},
        {1, 1, -1},
        {1, 1, 1},
        {-1, -1, 1},
        {1, -1, 1},
        {1, 1, 1},
        {-1, 1, 1}
    }));
    QVERIFY(mesh->normalArrayCount() == 1);
    QVERIFY((*mesh->normals(0) == vector<Vector3>{
        {1, 0, 0},
        {1, 0, 0},
        {1, 0, 0},
        {1, 0, 0},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1}
    }));
    QVERIFY(mesh->textureCoords2DArrayCount() == 2);
    QVERIFY((*mesh->textureCoords2D(0) == vector<Vector2>{
        {0.5, 1},
        {1, 1},
        {1, 0},
        {0.5, 0},
        {0, 1},
        {0.5, 1},
        {0.5, 0},
        {0, 0}
    }));
    QVERIFY(*mesh->textureCoords2D(1) == vector<Vector2>(8));
}

void ColladaImporterTest::material() {
    ColladaImporter importer;
    QVERIFY(importer.open(Directory::join(COLLADAIMPORTER_TEST_DIR, "material.dae")));

    QVERIFY(importer.materialCount() == 3);

    stringstream debug;
    Error::setOutput(&debug);
    QVERIFY(!importer.material(0));
    QVERIFY(debug.str() == "ColladaImporter: \"profile_GLSL\" effect profile not supported\n");

    debug.str("");
    QVERIFY(!importer.material(1));
    QVERIFY(debug.str() == "ColladaImporter: \"lambert\" shader not supported\n");

    QVERIFY(!!importer.material(2));
    PhongMaterialData* material = static_cast<PhongMaterialData*>(importer.material(2));

    QVERIFY(material->ambientColor() == Vector3(1, 0, 0));
    QVERIFY(material->diffuseColor() == Vector3(0, 1, 0));
    QVERIFY(material->specularColor() == Vector3(0, 0, 1));
    QCOMPARE(material->shininess(), 50.0f);
}

void ColladaImporterTest::image() {
    ColladaImporter importer;
    QVERIFY(importer.open(Directory::join(COLLADAIMPORTER_TEST_DIR, "image.dae")));

    QVERIFY(importer.image2DCount() == 2);

    stringstream debug;
    Error::setOutput(&debug);
    QVERIFY(!importer.image2D(0));
    Debug() << debug.str();
    QVERIFY(debug.str() == "ColladaImporter: \"image.jpg\" has unsupported format\n");

    QVERIFY(!!importer.image2D(1));
    ImageData2D* image = importer.image2D(1);

    /* Check only dimensions, as it is good enough proof that it is working */
    QVERIFY((image->dimensions() == Math::Vector2<GLsizei>(2, 3)));
}

}}}}
